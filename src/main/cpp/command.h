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

#ifndef __MODULE_COMMAND_H__
#define __MODULE_COMMAND_H__

#include "robotkernel/runnable.h"
#include "robotkernel/module_base.h"

#include "yaml-cpp/yaml.h"
#include "config.h"

namespace module_command {
#ifdef EMACS
}
#endif

class command :
    public std::enable_shared_from_this<command>,
    public robotkernel::module_base
{
    private:
        std::string cmd;

    public:
        //! yaml config construction
        /*!
         * \param name of jm 
         * \param node yaml node
         */
        command(const char* name, const YAML::Node& node);

        //! default destruction
        ~command();

        //! set module state
        /*
         * \param state module state to set
         * \return 0 on success
         */
        int set_state(module_state_t state);

        //! module trigger callback
        /*! does one measurement
         *
         * if log buffer is full, output thread is triggered
         */
        void tick();
};

#ifdef EMACS
{
#endif
};

#endif // __MODULE_COMMAND_H__

