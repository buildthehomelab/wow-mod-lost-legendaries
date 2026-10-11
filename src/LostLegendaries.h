/*
 * mod-lost-legendaries: settings shared between the module's source files.
 *
 * Released under the MIT License.
 */

#ifndef MOD_LOST_LEGENDARIES_H
#define MOD_LOST_LEGENDARIES_H

namespace LostLegendaries
{
    struct Config
    {
        bool enabled = true;
        bool announceAshbringer = true;
        bool steedFlying = true;
    };

    extern Config config;
}

#endif
