#!/usr/bin/env python3
"""
Fortran模块依赖分析工具
自动分析.f90文件之间的模块依赖关系，生成正确的编译顺序

用法: python3 analyze_fortran_deps.py <源码目录>
示例: python3 analyze_fortran_deps.py contrib/tran_heat/src
"""

import re
import os
import sys
from pathlib import Path
from collections import defaultdict, deque

def parse_fortran_file(filepath):
    """
    解析单个Fortran文件，提取：
    1. 它定义的模块名 (module xxx)
    2. 它依赖的模块名 (use xxx)
    """
    defined_modules = []
    used_modules = []
    
    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            # 去除注释
            line = line.split('!')[0].strip().lower()
            
            # 查找 module 定义（排除 module procedure）
            module_match = re.match(r'^\s*module\s+(\w+)', line)
            if module_match and 'procedure' not in line:
                defined_modules.append(module_match.group(1))
            
            # 查找 use 语句
            use_match = re.match(r'^\s*use\s+(\w+)', line)
            if use_match:
                used_modules.append(use_match.group(1))
    
    return defined_modules, used_modules

def analyze_directory(src_dir):
    """分析整个目录的Fortran文件依赖关系"""
    src_path = Path(src_dir)
    
    # 文件 -> 定义的模块列表
    file_defines = {}
    # 文件 -> 使用的模块列表
    file_uses = {}
    # 模块名 -> 定义它的文件
    module_to_file = {}
    
    # 收集所有.f90文件
    f90_files = sorted(src_path.glob('*.f90'))
    
    print(f"\n分析目录: {src_dir}")
    print(f"找到 {len(f90_files)} 个 .f90 文件\n")
    
    for fpath in f90_files:
        defined, used = parse_fortran_file(fpath)
        file_defines[fpath.name] = defined
        file_uses[fpath.name] = used
        
        for mod in defined:
            module_to_file[mod] = fpath.name
        
        print(f"文件: {fpath.name}")
        if defined:
            print(f"  定义模块: {', '.join(defined)}")
        if used:
            print(f"  使用模块: {', '.join(used)}")
        print()
    
    return file_defines, file_uses, module_to_file, [f.name for f in f90_files]

def build_dependency_graph(file_defines, file_uses, module_to_file):
    """构建文件依赖图"""
    # file -> 依赖的file列表
    dependencies = defaultdict(set)
    
    for fname, used_mods in file_uses.items():
        for mod in used_mods:
            if mod in module_to_file:
                dep_file = module_to_file[mod]
                if dep_file != fname:  # 排除自依赖
                    dependencies[fname].add(dep_file)
    
    return dependencies

def topological_sort(dependencies, all_files):
    """拓扑排序得到正确的编译顺序"""
    # 计算入度
    in_degree = {f: 0 for f in all_files}
    for deps in dependencies.values():
        for dep in deps:
            in_degree[dep] = in_degree.get(dep, 0) + 1
    
    # 队列：入度为0的文件（不依赖其他文件）
    queue = deque([f for f in all_files if in_degree[f] == 0])
    sorted_files = []
    
    while queue:
        fname = queue.popleft()
        sorted_files.append(fname)
        
        # 检查依赖于fname的文件
        for other_file in all_files:
            if fname in dependencies[other_file]:
                in_degree[other_file] -= 1
                if in_degree[other_file] == 0:
                    queue.append(other_file)
    
    # 检查是否有循环依赖
    if len(sorted_files) != len(all_files):
        remaining = set(all_files) - set(sorted_files)
        print(f"⚠️  警告：检测到循环依赖！涉及文件: {remaining}")
        # 将剩余文件添加到末尾
        sorted_files.extend(remaining)
    
    return sorted_files

def generate_makefile_snippet(sorted_files, src_dir, dependencies, output_file=None):
    """生成Makefile代码片段"""
    
    # 准备输出内容
    lines = []
    lines.append("# " + "="*70)
    lines.append("# 自动生成的Fortran模块依赖配置")
    lines.append("# 由 analyze_fortran_deps.py 自动生成")
    lines.append("# 请勿手动编辑此文件！")
    lines.append("# " + "="*70)
    lines.append("")
    
    lines.append("# 按依赖顺序列出的源文件")
    lines.append("tran_heat_f90srcfiles_ordered := \\")
    for fname in sorted_files:
        lines.append(f"    $(TRAN_HEAT_DIR)/src/{fname} \\")
    lines.append("")
    
    lines.append("tran_heat_f90srcfiles := $(tran_heat_f90srcfiles_ordered)")
    lines.append("")
    
    lines.append("# 显式依赖规则（确保模块编译顺序）")
    for fname in sorted_files:
        obj_file = f"$(TRAN_HEAT_DIR)/build/{fname[:-4]}.$(obj-suffix)"
        src_file = f"$(TRAN_HEAT_DIR)/src/{fname}"
        
        if fname in dependencies and dependencies[fname]:
            # 有依赖的文件
            dep_objs = [f"$(TRAN_HEAT_DIR)/build/{d[:-4]}.$(obj-suffix)" 
                       for d in dependencies[fname]]
            lines.append(f"{obj_file} : {src_file} \\")
            for dep in dep_objs:
                lines.append(f"    {dep} \\")
            lines.append("")
        else:
            # 无依赖的文件
            lines.append(f"{obj_file} : {src_file}")
    
    content = "\n".join(lines)
    
    # 如果指定了输出文件，写入文件
    if output_file:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"\n✅ Makefile已生成: {output_file}")
        print(f"   在主Makefile中使用: include {output_file}")
    else:
        # 否则打印到屏幕
        print("\n" + "="*60)
        print("生成的Makefile代码片段（复制到tran_heat.mk）:")
        print("="*60)
        print(content)
        print("\n" + "="*60)

def main():
    import argparse
    
    parser = argparse.ArgumentParser(
        description='Fortran模块依赖分析工具 - 自动分析并生成Makefile配置',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例用法:
  # 分析并显示依赖关系
  python3 analyze_fortran_deps.py contrib/tran_heat/src
  
  # 直接生成Makefile文件
  python3 analyze_fortran_deps.py contrib/tran_heat/src -o contrib/tran_heat_auto.mk
  
  # 静默模式（只生成文件，不显示详细信息）
  python3 analyze_fortran_deps.py contrib/tran_heat/src -o contrib/tran_heat_auto.mk -q
        """
    )
    
    parser.add_argument('src_dir', help='Fortran源码目录路径')
    parser.add_argument('-o', '--output', 
                       help='输出Makefile文件路径（不指定则打印到屏幕）')
    parser.add_argument('-q', '--quiet', action='store_true',
                       help='静默模式（只生成文件，不显示详细分析）')
    
    args = parser.parse_args()
    
    if not os.path.isdir(args.src_dir):
        print(f"错误：目录不存在: {args.src_dir}")
        sys.exit(1)
    
    # 分析文件
    file_defines, file_uses, module_to_file, all_files = analyze_directory(args.src_dir)
    
    # 构建依赖图
    dependencies = build_dependency_graph(file_defines, file_uses, module_to_file)
    
    if not args.quiet:
        print("\n依赖关系:")
        print("-" * 60)
        for fname, deps in sorted(dependencies.items()):
            if deps:
                print(f"{fname} 依赖于:")
                for dep in sorted(deps):
                    print(f"  -> {dep}")
        
        # 拓扑排序
        sorted_files = topological_sort(dependencies, all_files)
        
        print("\n正确的编译顺序:")
        print("-" * 60)
        for i, fname in enumerate(sorted_files, 1):
            print(f"{i}. {fname}")
    else:
        sorted_files = topological_sort(dependencies, all_files)
    
    # 生成Makefile代码
    generate_makefile_snippet(sorted_files, args.src_dir, dependencies, args.output)

if __name__ == "__main__":
    main()

