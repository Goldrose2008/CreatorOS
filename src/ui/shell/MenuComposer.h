#pragma once

class LocalizationService;
class Sidebar;

namespace MenuComposer
{
    void build(
        Sidebar *sidebar,
        const LocalizationService *localization
    );
}