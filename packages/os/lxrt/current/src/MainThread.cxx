/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:28 CEST 2004  MainThread.cxx 

                        MainThread.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/

#define OROBLD_OS_LXRT_INTERNAL
#include "os/MainThread.hpp"
//#include <linux/module.h>
#include <sched.h>
#include <sys/mman.h>
#include <iostream>
#include "os/fosi.h"
#include <unistd.h>
#include <sys/types.h>

// extern package config headers.
#include "pkgconf/system.h"
#ifdef OROPKG_CORELIB
#include "pkgconf/corelib.h"
#endif

#ifdef OROPKG_CORELIB_REPORTING
#include "corelib/Logger.hpp"
using ORO_CoreLib::Logger;
#endif

namespace ORO_OS
{
  SchedPolicy::SchedPolicy()
  {

      if ( getuid() != 0 ) {
	  std::cerr << "You are not root. This program requires that you are root." << std::endl;
	  exit(1);
      }

        /* check to see if rtai_lxrt module is loaded */
//         struct module_info modInfo;
//         size_t retSize;
//         if ( query_module("rtai_lxrt", QM_INFO, &modInfo, 
//                           sizeof(modInfo), &retSize) != 0 ) {
//             std::cerr <<"It appears the rtai_lxrt module is not loaded !"<<std::endl;
//             exit();
//         }
        struct sched_param param;
        param.sched_priority = 99;
        sched_setscheduler( SELECT_THIS_PID, OROSEM_OS_LXRT_SCHEDTYPE, &param);
        //init_linux_scheduler( OROSEM_OS_LXRT_SCHEDTYPE, 99);
        unsigned long name = nam2num("MAINTHREAD");
        if( !(rt_task = rt_task_init(name, 10,0,0)) ) // priority, stack, msg_size
            {
	      std::cerr << "Cannot rt_task_init() MainThread." << std::endl;
	      exit(1);
            }
                
        //rt_set_periodic_mode();
        //start_rt_timer( nano2count( usecs_to_nsecs(100) ) ); // 0.1ms = 100us

        // XXX Debugging, this works
        // BE SURE TO SET rt_preempt_always(1) when using one shot mode
        rt_set_oneshot_mode();
        rt_preempt_always(1);
        start_rt_timer(0);
#ifdef OROPKG_CORELIB_REPORTING
      Logger::log() << Logger::Debug << "Sched Policy : RTAI Task Created" << Logger::endl;
#endif
    }

    SchedPolicy::~SchedPolicy()
    {
        // we don't stop the timer
        //stop_rt_timer();
#ifdef OROPKG_CORELIB_REPORTING
      Logger::log() << Logger::Debug << "Sched Policy : Deleting RTAI task." << Logger::endl;
#endif
        rt_task_delete(rt_task);
    }

    MemPolicy::MemPolicy(int hp, int stk)
        : alloc_heap(hp), alloc_stack(hp)
    {
        // stack, heap : see touchall.c in lxrt/lib
        // lock_all(stk,hp);
        mlockall(MCL_CURRENT | MCL_FUTURE );
    }

    MainThread* MainThread::mt;
}
