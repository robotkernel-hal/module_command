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

#include "command.h"
#include "robotkernel/helpers.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "yaml-cpp/yaml.h"

MODULE_DEF(module_command, module_command::command)

using namespace std;
using namespace std::placeholders;
using namespace robotkernel;
using namespace module_command;
        
//! yaml config construction
/*!
 * \param name name of jm
 * \param node yaml node
 */
command::command(const char* name, const YAML::Node& node) :
    module_base("module_command", name, node),
    trigger_base(node["trigger"] ? node["trigger"] : YAML::Node())
{
    trigger_dev_name = get_as<string>(node, "trigger_dev");

    exec_on_switch_to_op = get_as<bool>(node, "exec_on_switch_to_op", false);
    cmd = get_as<string>(node, "command", string(""));
    stop_trace = get_as<bool>(node, "stop_trace", false);
    if (cmd == "stop-trace")
        stop_trace = true;
    allow_concurrent_executions = get_as<bool>(node, "allow_concurrent_executions", false);

    running_command = -1;
}

//! default destruction
command::~command() {
}

void command::tick() {
    if (stop_trace) {
        log(info, "stopping trace...\n");
        int fd = open("/sys/kernel/debug/tracing/tracing_on", O_WRONLY);
        if (fd == -1)
            log(error, "could not open tracing_on-file: %d %s\n", errno, strerror(errno));
        else {
            std::string data = "0\n";
            ssize_t ret = write(fd, data.c_str(), data.size());
            if (ret != (signed)data.size())
                log(error, "could not write to tracing_on-file: ret %d, %d %s\n", (int)ret, errno, strerror(errno));
            close(fd);
        }
    }

    if (cmd != "stop-trace") {
        if (!allow_concurrent_executions && running_command != -1) {
            int status;
            pid_t ret = waitpid(running_command, &status, WNOHANG);
            if (ret != running_command) {
                log(info, "last command still running. ignoring trigger!\n");
                return;
            }
            running_command = -1;
        }

        log(info, "execute command: %s\n", cmd.c_str());

        pid_t ret = fork();
        if (ret == -1) {
            log(error, "failed to execute command: fork(): %d %s\n", errno, strerror(errno));
            return;
        }
        if (ret == 0) {
            system(cmd.c_str());
            exit(0);
        }
        running_command = ret;
    }
}
       

//! State transition from SAFEOP to PREOP
void command::set_state_safeop_2_preop() {
    trigger_dev->remove_trigger(static_pointer_cast<trigger_base>(shared_from_this()));
    trigger_dev = nullptr;
}

//! State transition from PREOP to SAFEOP
void command::set_state_preop_2_safeop() {
    trigger_dev = get_device<trigger>(trigger_dev_name);
    trigger_dev->add_trigger(static_pointer_cast<trigger_base>(shared_from_this()));
}

//! State transition from PREOP to SAFEOP
void command::set_state_safeop_2_op() {
    if (exec_on_switch_to_op) {
        log(info, "execute command: %s\n", cmd.c_str());
        system(cmd.c_str());
    }
}

