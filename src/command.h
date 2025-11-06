//! robotkernel module command
/*!
 * author: Robert Burger <robert.burger@dlr.de>
 */

/*
 * This file is part of robotkernel.
 *
 * robotkernel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * robotkernel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with robotkernel.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MODULE_COMMAND__COMMAND_H
#define MODULE_COMMAND__COMMAND_H

#include <sys/types.h>

#include "robotkernel/runnable.h"
#include "robotkernel/trigger_base.h"
#include "robotkernel/module_base.h"

#include "yaml-cpp/yaml.h"
#include "config.h"

using namespace robotkernel;

namespace module_command {

class command :
    public std::enable_shared_from_this<command>,
    public trigger_base,
    public module_base
{
    private:
        bool exec_on_switch_to_op;
        std::string cmd;
        bool stop_trace;
        bool allow_concurrent_executions;

        std::string trigger_dev_name;
        std::shared_ptr<trigger> trigger_dev;

        pid_t running_command;

    public:
        //! yaml config construction
        /*!
         * \param name of jm 
         * \param node yaml node
         */
        command(const char* name, const YAML::Node& node);

        //! default destruction
        ~command();
        
        //! State transition from SAFEOP to PREOP
        virtual void set_state_safeop_2_preop() override;

        //! State transition from PREOP to SAFEOP
        virtual void set_state_preop_2_safeop() override;

        //! State transition from PREOP to SAFEOP
        virtual void set_state_safeop_2_op() override;

        //! module trigger callback
        /*! does one measurement
         *
         * if log buffer is full, output thread is triggered
         */
        virtual void tick() override;
};

};

#endif // MODULE_COMMAND__COMMAND_H

