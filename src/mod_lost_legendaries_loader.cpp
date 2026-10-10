/*
 * mod-lost-legendaries loader.
 *
 * AzerothCore looks up a loader symbol derived from the module's folder name: for folder
 * "mod-lost-legendaries" that symbol is exactly "Addmod_lost_legendariesScripts". If you clone
 * the repo under a different folder name, rename this function to match.
 *
 * Released under the MIT License.
 */

void AddLostLegendariesScripts();
void AddLostLegendariesAshbringerScripts();

void Addmod_lost_legendariesScripts()
{
    AddLostLegendariesScripts();
    AddLostLegendariesAshbringerScripts();
}
