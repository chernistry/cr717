#pragma once

#include <juce_core/juce_core.h>
#include <map>
#include <string>

struct Preset
{
    juce::String name;
    juce::String style;
    int bpm = 120;
    float swing = 0.0f;
    std::map<juce::String, float> parameters;
    std::array<std::array<bool, 16>, 12> pattern{}; // 12 voices x 16 steps
    
    static Preset fromJson(const juce::var& json)
    {
        Preset preset;
        preset.name = json["name"].toString();
        preset.style = json["style"].toString();
        preset.bpm = json["bpm"];
        preset.swing = json["swing"];
        
        if (auto* params = json["parameters"].getArray())
        {
            for (const auto& param : *params)
            {
                juce::String id = param["id"].toString();
                float value = param["value"];
                preset.parameters[id] = value;
            }
        }
        
        return preset;
    }
    
    juce::var toJson() const
    {
        auto* obj = new juce::DynamicObject();
        obj->setProperty("name", name);
        obj->setProperty("style", style);
        obj->setProperty("bpm", bpm);
        obj->setProperty("swing", swing);
        
        juce::Array<juce::var> params;
        for (const auto& [id, value] : parameters)
        {
            auto* param = new juce::DynamicObject();
            param->setProperty("id", id);
            param->setProperty("value", value);
            params.add(juce::var(param));
        }
        obj->setProperty("parameters", params);
        
        return juce::var(obj);
    }
};

class PresetManager
{
public:
    PresetManager()
    {
        loadFactoryPresets();
    }
    
    const std::vector<Preset>& getPresets() const { return presets; }
    
    std::vector<Preset> getPresetsByStyle(const juce::String& style) const
    {
        std::vector<Preset> filtered;
        for (const auto& preset : presets)
            if (preset.style == style)
                filtered.push_back(preset);
        return filtered;
    }
    
    const Preset* getPreset(const juce::String& name) const
    {
        for (const auto& preset : presets)
            if (preset.name == name)
                return &preset;
        return nullptr;
    }
    
private:
    std::vector<Preset> presets;
    
    void loadFactoryPresets()
    {
        // UK Garage / 2-Step
        presets.push_back(createUKGPreset("UKG Classic", 135, 0.58f));
        presets.push_back(createUKGPreset("2-Step Shuffle", 138, 0.62f));
        presets.push_back(createUKGPreset("UKG Deep", 132, 0.56f));
        presets.push_back(createUKGPreset("Garage Minimal", 136, 0.54f));
        presets.push_back(createUKGPreset("UKG Skippy", 137, 0.60f));
        presets.push_back(createUKGPreset("2-Step Dark", 134, 0.58f));
        
        // Drum & Bass
        presets.push_back(createDnBPreset("DnB Neurofunk", 174, 0.0f));
        presets.push_back(createDnBPreset("Liquid DnB", 172, 0.0f));
        presets.push_back(createDnBPreset("Jump Up", 176, 0.0f));
        presets.push_back(createDnBPreset("DnB Minimal", 170, 0.0f));
        presets.push_back(createDnBPreset("Jungle Classic", 173, 0.0f));
        presets.push_back(createDnBPreset("DnB Halftime", 175, 0.0f));
        
        // Techno
        presets.push_back(createTechnoPreset("Techno Peak", 132, 0.0f));
        presets.push_back(createTechnoPreset("Minimal Techno", 128, 0.0f));
        presets.push_back(createTechnoPreset("Hard Techno", 136, 0.0f));
        presets.push_back(createTechnoPreset("Techno Hypnotic", 130, 0.0f));
        presets.push_back(createTechnoPreset("Industrial", 134, 0.0f));
        presets.push_back(createTechnoPreset("Techno Groove", 129, 0.0f));
        
        // House / Deep House
        presets.push_back(createHousePreset("Deep House", 122, 0.54f));
        presets.push_back(createHousePreset("House Classic", 124, 0.52f));
        presets.push_back(createHousePreset("Tech House", 126, 0.0f));
        presets.push_back(createHousePreset("Jackin House", 125, 0.56f));
        presets.push_back(createHousePreset("Soulful House", 120, 0.55f));
        presets.push_back(createHousePreset("House Minimal", 123, 0.52f));
        
        // Trap / Hip-Hop
        presets.push_back(createTrapPreset("Trap Hard", 140, 0.0f));
        presets.push_back(createTrapPreset("Trap Rolls", 145, 0.0f));
        presets.push_back(createTrapPreset("Hip-Hop Boom", 75, 0.0f));
        presets.push_back(createTrapPreset("Trap Minimal", 142, 0.0f));
        presets.push_back(createTrapPreset("Hip-Hop Chill", 80, 0.0f));
        presets.push_back(createTrapPreset("Trap Dark", 150, 0.0f));
        
        // Lo-Fi
        presets.push_back(createLoFiPreset("Lo-Fi Chill", 85, 0.0f));
        presets.push_back(createLoFiPreset("Lo-Fi Study", 78, 0.0f));
        presets.push_back(createLoFiPreset("Lo-Fi Jazzy", 92, 0.0f));
        presets.push_back(createLoFiPreset("Lo-Fi Dusty", 72, 0.0f));
        presets.push_back(createLoFiPreset("Lo-Fi Tape", 88, 0.0f));
        presets.push_back(createLoFiPreset("Lo-Fi Ambient", 96, 0.0f));
    }
    
    Preset createUKGPreset(const juce::String& name, int bpm, float swing)
    {
        Preset p;
        p.name = name;
        p.style = "UK Garage";
        p.bpm = bpm;
        p.swing = swing;
        p.parameters["bdLevel"] = 0.85f;
        p.parameters["sdLevel"] = 0.75f;
        p.parameters["chLevel"] = 0.55f;
        p.parameters["ohLevel"] = 0.45f;
        p.parameters["cpLevel"] = 0.65f;
        p.parameters["bdSendA"] = 0.15f;
        p.parameters["sdSendA"] = 0.25f;
        p.parameters["sdSendB"] = 0.15f;
        p.parameters["reverbSize"] = 0.45f;
        p.parameters["delayTime"] = 0.375f;
        
        // UK Garage pattern: kick on 1 and 3.5, snare on 2 and 4, hats on offbeats
        p.pattern[0] = {1,0,0,0, 0,0,1,0, 0,0,0,0, 0,0,0,0}; // BD
        p.pattern[1] = {0,0,0,0, 1,0,0,0, 0,0,0,0, 1,0,0,0}; // SD
        p.pattern[7] = {0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1}; // CH
        
        return p;
    }
    
    Preset createDnBPreset(const juce::String& name, int bpm, float swing)
    {
        Preset p;
        p.name = name;
        p.style = "Drum & Bass";
        p.bpm = bpm;
        p.swing = swing;
        p.parameters["bdLevel"] = 0.9f;
        p.parameters["sdLevel"] = 0.85f;
        p.parameters["chLevel"] = 0.6f;
        p.parameters["ohLevel"] = 0.5f;
        p.parameters["bdSendA"] = 0.1f;
        p.parameters["sdSendA"] = 0.2f;
        p.parameters["reverbSize"] = 0.35f;
        return p;
    }
    
    Preset createTechnoPreset(const juce::String& name, int bpm, float swing)
    {
        Preset p;
        p.name = name;
        p.style = "Techno";
        p.bpm = bpm;
        p.swing = swing;
        p.parameters["bdLevel"] = 0.95f;
        p.parameters["sdLevel"] = 0.7f;
        p.parameters["chLevel"] = 0.65f;
        p.parameters["ohLevel"] = 0.4f;
        p.parameters["cpLevel"] = 0.6f;
        p.parameters["bdSendA"] = 0.05f;
        p.parameters["sdSendA"] = 0.15f;
        p.parameters["reverbSize"] = 0.3f;
        return p;
    }
    
    Preset createHousePreset(const juce::String& name, int bpm, float swing)
    {
        Preset p;
        p.name = name;
        p.style = "House";
        p.bpm = bpm;
        p.swing = swing;
        p.parameters["bdLevel"] = 0.9f;
        p.parameters["sdLevel"] = 0.75f;
        p.parameters["chLevel"] = 0.6f;
        p.parameters["ohLevel"] = 0.5f;
        p.parameters["bdSendA"] = 0.1f;
        p.parameters["sdSendA"] = 0.2f;
        p.parameters["reverbSize"] = 0.4f;
        return p;
    }
    
    Preset createTrapPreset(const juce::String& name, int bpm, float swing)
    {
        Preset p;
        p.name = name;
        p.style = "Trap";
        p.bpm = bpm;
        p.swing = swing;
        p.parameters["bdLevel"] = 0.95f;
        p.parameters["sdLevel"] = 0.8f;
        p.parameters["chLevel"] = 0.7f;
        p.parameters["ohLevel"] = 0.3f;
        p.parameters["cpLevel"] = 0.7f;
        p.parameters["bdSendA"] = 0.15f;
        p.parameters["sdSendA"] = 0.25f;
        p.parameters["reverbSize"] = 0.5f;
        return p;
    }
    
    Preset createLoFiPreset(const juce::String& name, int bpm, float swing)
    {
        Preset p;
        p.name = name;
        p.style = "Lo-Fi";
        p.bpm = bpm;
        p.swing = swing;
        p.parameters["bdLevel"] = 0.7f;
        p.parameters["sdLevel"] = 0.65f;
        p.parameters["chLevel"] = 0.45f;
        p.parameters["ohLevel"] = 0.35f;
        p.parameters["bdSendA"] = 0.25f;
        p.parameters["sdSendA"] = 0.3f;
        p.parameters["reverbSize"] = 0.6f;
        return p;
    }
};
