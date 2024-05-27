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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "yaml-cpp/yaml.h"
#include <string_util/string_util.h>

MODULE_DEF(module_command, module_command::command)

using namespace std;
using namespace std::placeholders;
using namespace robotkernel;
using namespace module_command;
using namespace string_util;
        
//! yaml config construction
/*!
 * \param name name of jm
 * \param node yaml node
 */
command::command(const char* name, const YAML::Node& node) 
    : module_base("module_command", name, node)
{
    exec_on_switch_to_op = get_as<bool>(node, "exec_on_switch_to_op", false);
    cmd = get_as<string>(node, "command", string(""));
    stop_trace = get_as<bool>(node, "stop_trace", false);
    if (cmd == "stop-trace")
	    stop_trace = true;

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
	    log(info, "execute command: %s\n", cmd.c_str());

	    pid_t ret = fork();
	    if (ret == 0) {
		    system(cmd.c_str());
		    exit(0);
	    }
    }
}

int command::set_state(module_state_t state) {
    // get transition
    uint32_t transition = GEN_STATE(this->state, state);
    
    log(info, "state %s requested\n", state_to_string(state));

    switch (transition) {
        case op_2_safeop:
        case op_2_preop:
        case op_2_init:
        case op_2_boot:
            // ====> stop sending commands
            if (state == module_state_safeop)
                break;
        case safeop_2_preop:
        case safeop_2_init:
        case safeop_2_boot:
            // ====> stop receiving measurements
            if (state == module_state_preop)
                break;
        case preop_2_init:
        case preop_2_boot:
            // ====> deinit devices
        case init_2_init:
            // ====> re-/open device
            if (state == module_state_init)
                break;
        case init_2_boot:
            break;
        case boot_2_init:
        case boot_2_preop:
        case boot_2_safeop:
        case boot_2_op:
            // ====> re-/open device
            if (state == module_state_init)
                break;
        case init_2_op:
        case init_2_safeop:
        case init_2_preop:
            // ====> initial devices            
            if (state == module_state_preop)
                break;
        case preop_2_op:
        case preop_2_safeop:
            // ====> start receiving measurements
            if (state == module_state_safeop)
                break;
        case safeop_2_op:
            // ====> start sending commands
            if (exec_on_switch_to_op) {
                log(info, "execute command: %s\n", cmd.c_str());
                system(cmd.c_str());
            }
            break;
        case op_2_op:
        case safeop_2_safeop:
        case preop_2_preop:
            // ====> do nothing
            break;

        default:
            break;
    }

    return (this->state = state);
}

