//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

/****************************************************************/
/*                  Reactor Physics Application                 */
/*                        Main Program                          */
/*              堆物理应用程序 - 主程序入口                     */
/****************************************************************/

#include "mooseprojectsApp.h"
#include "MooseInit.h"
#include "Moose.h"
#include "MooseApp.h"
#include "AppFactory.h"

// C++ Standard Library
// C++标准库
#include <iostream>

int main(int argc, char * argv[])
{
  // Initialize MPI and MOOSE
  // 初始化MPI和MOOSE
  MooseInit init(argc, argv);
  
  // Register applications
  // 注册应用程序
  mooseprojectsApp::registerApps();
  
  // Create application instance
  // 创建应用程序实例
  std::shared_ptr<MooseApp> app = AppFactory::createAppShared("mooseprojectsApp", argc, argv);
  
  // Execute application
  // 执行应用程序
  app->run();
  
  return 0;
}
