#pragma warning(push)
#pragma warning(disable : 4099 4996 5054)
#include "SKSEMenuFramework.h"
#pragma warning(pop)

#include <SimpleIni.h>

namespace {
    constexpr auto          kIniPath = "Data/SKSE/Plugins/HideUI.ini";
    constexpr std::uint32_t kDefaultHotkey = 0x42;  // F8

    std::atomic<std::uint32_t> g_hotkey{kDefaultHotkey};
    std::atomic<bool>          g_capturing{false};

    void LoadSettings() {
        CSimpleIniA ini;
        ini.SetUnicode();
        if (ini.LoadFile(kIniPath) < 0) return;

        const auto value = ini.GetLongValue("General", "Hotkey", kDefaultHotkey);
        g_hotkey = value >= 0 && value < SKSE::InputMap::kMaxMacros ? static_cast<std::uint32_t>(value) : 0;
    }

    void SaveSettings() {
        CSimpleIniA ini;
        ini.SetUnicode();
        ini.LoadFile(kIniPath);
        ini.SetLongValue("General", "Hotkey", static_cast<long>(g_hotkey.load()));
        if (ini.SaveFile(kIniPath) < 0) SKSE::log::warn("Couldn't write {}", kIniPath);
    }

    void Toggle() {
        if (auto* ui = RE::UI::GetSingleton()) ui->ShowMenus(!ui->IsShowingMenus());
    }

    std::uint32_t KeyCode(const RE::ButtonEvent& button) {
        const auto code = button.GetIDCode();
        switch (button.GetDevice()) {
            case RE::INPUT_DEVICE::kKeyboard:
                return code;
            case RE::INPUT_DEVICE::kMouse:
                return code + SKSE::InputMap::kMacro_MouseButtonOffset;
            case RE::INPUT_DEVICE::kGamepad: {
                const auto key = SKSE::InputMap::GamepadMaskToKeycode(code);
                return key < SKSE::InputMap::kMaxMacros ? key : 0;
            }
            default:
                return 0;
        }
    }

    class InputSink final : public RE::BSTEventSink<RE::InputEvent*> {
    public:
        RE::BSEventNotifyControl ProcessEvent(
            RE::InputEvent* const* events, RE::BSTEventSource<RE::InputEvent*>*) override {
            const auto hotkey = g_hotkey.load(std::memory_order_relaxed);
            if (!events || hotkey == 0) return RE::BSEventNotifyControl::kContinue;

            for (auto* event = *events; event; event = event->next) {
                const auto* button = event->AsButtonEvent();
                if (button && button->IsDown() && KeyCode(*button) == hotkey) {
                    Toggle();
                    break;
                }
            }
            return RE::BSEventNotifyControl::kContinue;
        }
    };

    InputSink g_inputSink;

    // Returning true drops the event before the game or the menu sees it.
    bool __stdcall OnInput(RE::InputEvent* event) {
        if (!g_capturing) return false;

        const auto* button = event ? event->AsButtonEvent() : nullptr;
        if (!button) return false;

        // Left and right click stay with the menu.
        const auto code = KeyCode(*button);
        if (code == 0 || code == SKSE::InputMap::kMacro_MouseButtonOffset ||
            code == SKSE::InputMap::kMacro_MouseButtonOffset + 1)
            return false;

        if (button->IsDown()) {
            g_capturing = false;
            if (code != REX::W32::DIK_ESCAPE) {
                g_hotkey = code;
                SaveSettings();
            }
        }
        return true;
    }

    void __stdcall OnMenuEvent(SKSEMenuFramework::Model::EventType type) {
        if (type == SKSEMenuFramework::Model::kCloseMenu) g_capturing = false;
    }

    void __stdcall Render() {
        static std::uint32_t named = UINT32_MAX;
        static std::string   label;

        ImGuiMCP::AlignTextToFramePadding();
        ImGuiMCP::TextUnformatted("Hotkey");
        ImGuiMCP::SameLine();

        if (g_capturing) {
            ImGuiMCP::Button("Press a key, Esc to cancel##hotkey");
            return;
        }

        if (const auto hotkey = g_hotkey.load(); hotkey != named) {
            named = hotkey;
            auto name = hotkey ? SKSE::InputMap::GetKeyName(hotkey) : "None"s;
            label = (name.empty() ? std::format("Key {}", hotkey) : name) + "##hotkey";
        }
        if (ImGuiMCP::Button(label.c_str())) g_capturing = true;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    LoadSettings();

    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
        switch (message->type) {
            case SKSE::MessagingInterface::kInputLoaded:
                if (auto* input = RE::BSInputDeviceManager::GetSingleton()) input->AddEventSink(&g_inputSink);
                break;
            case SKSE::MessagingInterface::kDataLoaded:
                if (SKSEMenuFramework::IsInstalled()) {
                    SKSEMenuFramework::SetSection("Hide UI");
                    SKSEMenuFramework::AddSectionItem("Settings", Render);
                    SKSEMenuFramework::AddInputEvent(OnInput);
                    SKSEMenuFramework::AddEvent(OnMenuEvent, 0.0f);
                }
                break;
            default:
                break;
        }
    });

    return true;
}
