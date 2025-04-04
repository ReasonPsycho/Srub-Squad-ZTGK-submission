//
// Created by cheily on 21.03.2024.
//

#include "Util.h"
#include "ECS/Unit/Unit.h"
#include "ECS/HUD/Components/Text.h"
#include "ECS/HUD/Components/Sprite.h"
#include "ECS/HUD/Interactables/HUDSlider.h"
#include "ECS/HUD/HUD.h"
#include "ECS/Unit/UnitSystem.h"

namespace ztgk {

    long long time() {
        return std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    Console ztgk::console = Console();

    void update_unit_hud() {
        // this should only happen once due to path re-load guard in Sprite.load
        auto allies = ztgk::game::scene->systemManager.getSystem<UnitSystem>()->unitComponents | ranges::views::filter([](Unit * unit){ return unit->isAlly; }) | std::ranges::to<std::vector<Unit *>>();
        static auto eactions = ztgk::game::scene->getChild("HUD")->getChild("Game")->getChild("Action Panel");
        auto hud = ztgk::game::scene->systemManager.getSystem<HUD>();

        if (!allies.empty()) {
            for (int i = 0; i < 3; ++i) {
                eactions->children[i]->getComponent<Text>()->content = allies[i]->name;
                eactions->children[i]->getComponent<Sprite>()->load(allies[i]->icon_path);
                eactions->children[i]->getChild("Display Bar")->getComponent<HUDSlider>()->set(
                        allies[i]->stats.hp / (allies[i]->stats.max_hp + allies[i]->stats.added.max_hp));
                if (!allies[i]->isAlive) eactions->children[i]->getComponent<Sprite>()->color = ztgk::color.GRAY;
                // came back to life
                if (allies[i]->isAlive && eactions->children[i]->getComponent<Sprite>()->color == ztgk::color.GRAY)
                    eactions->children[i]->getComponent<Sprite>()->color = ztgk::color.WHITE;
                if (allies[i]->isSelected) eactions->children[i]->getComponent<Sprite>()->color = ztgk::color.GREEN;
                // deselected
                if (!allies[i]->isSelected && eactions->children[i]->getComponent<Sprite>()->color == ztgk::color.GREEN)
                    eactions->children[i]->getComponent<Sprite>()->color = ztgk::color.WHITE;
                //            else eactions->children[i]->getComponent<Sprite>()->color = ztgk::color.WHITE;
                eactions->children[i]->getChild("Emote")->getComponent<Sprite>()->load(ztgk::game::emotes[allies[i]->mostRecentEmote]->path);
            }
            for (int i = 3; i < 6; ++i) {
                static std::vector<unsigned> cd_groups = {ztgk::game::ui_data.gr_act_cd11, ztgk::game::ui_data.gr_act_cd21, ztgk::game::ui_data.gr_act_cd31};

                if (!allies[i % 3]->equipment.item1 && !allies[i % 3]->equipment.item2) {
                    eactions->children[i]->children[0]->getComponent<Sprite>()->load(allies[i % 3]->equipment.item0->icon_path);
                    eactions->children[i]->getChild("Display Bar")->getComponent<HUDSlider>()->set(
                            allies[i % 3]->equipment.item0->cd_sec / allies[i % 3]->equipment.item0->stats.cd_max_sec);
                    hud->getGroupOrDefault(cd_groups[i % 3])->setHidden(false);
                } else if (allies[i % 3]->equipment.item1) {
                    eactions->children[i]->children[0]->getComponent<Sprite>()->load(allies[i % 3]->equipment.item1->icon_path);
                    eactions->children[i]->getChild("Display Bar")->getComponent<HUDSlider>()->set(
                            allies[i % 3]->equipment.item1->cd_sec / allies[i % 3]->equipment.item1->stats.cd_max_sec);
                    hud->getGroupOrDefault(cd_groups[i % 3])->setHidden(false);
                } else {
                    eactions->children[i]->children[0]->getComponent<Sprite>()->load("res/textures/question_mark.png");
                    hud->getGroupOrDefault(cd_groups[i % 3])->setHidden(true);
                }
            }
            for (int i = 6; i < 9; ++i) {
                static std::vector<unsigned> cd_groups = {ztgk::game::ui_data.gr_act_cd12, ztgk::game::ui_data.gr_act_cd22, ztgk::game::ui_data.gr_act_cd32};

                if (allies[i % 3]->equipment.item2) {
                    eactions->children[i]->children[0]->getComponent<Sprite>()->load(allies[i % 3]->equipment.item2->icon_path);
                    eactions->children[i]->getChild("Display Bar")->getComponent<HUDSlider>()->set(
                            allies[i % 3]->equipment.item2->cd_sec / allies[i % 3]->equipment.item2->stats.cd_max_sec);
                    hud->getGroupOrDefault(cd_groups[i % 3])->setHidden(false);
                } else {
                    eactions->children[i]->children[0]->getComponent<Sprite>()->load("res/textures/question_mark.png");
                    hud->getGroupOrDefault(cd_groups[i % 3])->setHidden(true);
                }

            }
        }



        // todo determine group hud
        Unit * unit;
        auto unitsys = ztgk::game::scene->systemManager.getSystem<UnitSystem>();
        auto fnd = std::find_if(unitsys->unitComponents.begin(),
                                 unitsys->unitComponents.end(),
                                 [](auto &unit) { return unit->uniqueID == ztgk::game::ui_data.tracked_unit_id; });
        // if not found
        if (fnd == unitsys->unitComponents.end()) {
            // and there are no selected units
            if (unitsys->selectedUnits.empty()) {
                // hide the layer
                ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_middle)->setHidden(true);
                return;
            } else {
                // otherwise use the first selected unit
                unit = unitsys->selectedUnits[0];
                ztgk::game::ui_data.tracked_unit_id = unit->uniqueID;
            }
        } else {
            // or just the found one
            unit = *fnd;
        }
        ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_middle)->setHidden(false);

        auto eunit = ztgk::game::scene->getChild("HUD")->getChild("Game")->getChild("Unit Details");

        auto ent = eunit->getChild("Portrait");
        ent->getComponent<Sprite>()->load(unit->icon_path);

        ent = eunit->getChild("Name");
        ent->getComponent<Text>()->content = unit->name;

        auto emods = eunit->getChild("Mods");
        emods->getChild("ATK")->getComponent<Text>()->content = std::format("{} + {}", unit->stats.added.dmg_perc, unit->stats.added.dmg_flat);
        emods->getChild("DEF")->getComponent<Text>()->content = std::format("{} + {}", unit->stats.added.def_perc, unit->stats.added.def_flat);
        emods->getChild("CD")->getComponent<Text>()->content = std::format("{}", unit->stats.added.atk_speed);
        emods->getChild("RNG")->getComponent<Text>()->content = std::format("{}", unit->stats.added.rng_add);
        emods->getChild("MNSP")->getComponent<Text>()->content = std::format("{}", unit->stats.mine_spd + unit->stats.added.mine_speed);
        emods->getChild("MVSP")->getComponent<Text>()->content = std::format("{}", unit->stats.move_spd + unit->stats.added.move_speed);

        eunit->getChild("Display Bar")->getComponent<HUDSlider>()->displayMax = unit->stats.max_hp + unit->stats.added.max_hp;
        eunit->getChild("Display Bar")->getComponent<HUDSlider>()->set_in_display_range(unit->stats.hp);

        update_weapon_hud(unit);
    }

    void update_weapon_hud(Unit *unit) {
        Item * item = unit->equipment.item1;
        if (!item && unit->equipment.item2 && unit->equipment.item2->takesTwoSlots) {
            item = unit->equipment.item2;
        }
        if (item) {
            auto eitem1 = ztgk::game::scene->getChild("HUD")->getChild("Game")->getChild("Unit Details")->getChild("Weapon Portrait #1");
            eitem1->getComponent<Text>()->content = item->name;
            eitem1->getComponent<Sprite>()->load(item->icon_path);
            if (item->offensive || item->active) {
                ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w1_offensive)->setHidden(false);
                ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w1_passive)->setHidden(true);
    
                eitem1->getChild("Offensive Stats")->getChild("ATK")->getComponent<Text>()->content = std::format("{}",item->stats.dmg);
                // if there are more active items than just the beacon this needs to work similar to below or just make a new group
                if (item->offensive) eitem1->getChild("Offensive Stats")->getChild("ATK")->getComponent<Sprite>()->load("res/textures/icons/stat/atk.png");
                else if (item->active) eitem1->getChild("Offensive Stats")->getChild("ATK")->getComponent<Sprite>()->load("res/textures/icons/stat/hp.png");
                eitem1->getChild("Offensive Stats")->getChild("RNG")->getComponent<Text>()->content = std::format("{}",item->stats.range.add);
                eitem1->getChild("Offensive Stats")->getChild("CD")->getChild("Display Bar")->getComponent<HUDSlider>()->displayMax = item->stats.cd_max_sec;
                eitem1->getChild("Offensive Stats")->getChild("CD")->getChild("Display Bar")->getComponent<HUDSlider>()->set_in_display_range(item->cd_sec);
            } else {
                ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w1_offensive)->setHidden(true);
                ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w1_passive)->setHidden(false);
    
                auto ent = eitem1->getChild("Passive Stats");
                for (int i = 1; i < 5; ++i) {
                    ztgk::game::scene->systemManager.getSystem<HUD>()->findGroupByName("Weapon 1 STAT " + to_string(i))->setHidden(true);
                }
                int i = 1;
                for (auto stats : item->highlight_passive_stats) {
                    if (i > 5) break;
                    string ent_name = "STAT" + to_string(i);
                    ent->getChild(ent_name)->getComponent<Sprite>()->load(stats.first);
                    ent->getChild(ent_name)->getComponent<Text>()->content = stats.second;
                    ztgk::game::scene->systemManager.getSystem<HUD>()->findGroupByName("Weapon 1 STAT " + to_string(i))->setHidden(false);
                    i++;
                }
            }
        } else {
            auto eitem1 = ztgk::game::scene->getChild("HUD")->getChild("Game")->getChild("Unit Details")->getChild("Weapon Portrait #1");
            eitem1->getComponent<Text>()->content = "*No Item*";
            if (unit->equipment.item2) // todo smarter slot select for hands, for now just check if the icon will be swapped to hands icon and don't load question mark if so
                eitem1->getComponent<Sprite>()->load("res/textures/question_mark.png");
            eitem1->getChild("Offensive Stats")->getChild("ATK")->getComponent<Sprite>()->load("res/textures/icons/stat/atk.png");
            ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w1_offensive)->setHidden(true);
            ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w1_passive)->setHidden(true);
        }

        item = unit->equipment.item2;
        if (!item && unit->equipment.item1 && unit->equipment.item1->takesTwoSlots) {
            item = unit->equipment.item1;
        }
        if (item) {
            auto eitem2 = ztgk::game::scene->getChild("HUD")->getChild("Game")->getChild("Unit Details")->getChild("Weapon Portrait #2");
            eitem2->getComponent<Text>()->content = item->name;
            eitem2->getComponent<Sprite>()->load(item->icon_path);
            if (item->offensive || item->active) {
                ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w2_offensive)->setHidden(false);
                ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w2_passive)->setHidden(true);
    
                eitem2->getChild("Offensive Stats")->getChild("ATK")->getComponent<Text>()->content = std::format("{}",item->stats.dmg);
                // if there are more active items than just the beacon this needs to work similar to below or just make a new group
                if (item->offensive) eitem2->getChild("Offensive Stats")->getChild("ATK")->getComponent<Sprite>()->load("res/textures/icons/stat/atk.png");
                else if (item->active) eitem2->getChild("Offensive Stats")->getChild("ATK")->getComponent<Sprite>()->load("res/textures/icons/stat/hp.png");
                eitem2->getChild("Offensive Stats")->getChild("RNG")->getComponent<Text>()->content = std::format("{}",item->stats.range.add);
                eitem2->getChild("Offensive Stats")->getChild("CD")->getChild("Display Bar")->getComponent<HUDSlider>()->displayMax = item->stats.cd_max_sec;
                eitem2->getChild("Offensive Stats")->getChild("CD")->getChild("Display Bar")->getComponent<HUDSlider>()->set_in_display_range(item->cd_sec);
            } else {
                ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w2_offensive)->setHidden(true);
                ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w2_passive)->setHidden(false);
    
                auto ent = eitem2->getChild("Passive Stats");
                for (int i = 1; i < 5; ++i) {
                    ztgk::game::scene->systemManager.getSystem<HUD>()->findGroupByName("Weapon 2 STAT " + to_string(i))->setHidden(true);
                }

                int i = 1;
                for (auto stats : item->highlight_passive_stats) {
                    if (i > 4) break;
                    string ent_name = "STAT" + to_string(i);
                    ent->getChild(ent_name)->getComponent<Sprite>()->load(stats.first);
                    ent->getChild(ent_name)->getComponent<Text>()->content = stats.second;
                    ztgk::game::scene->systemManager.getSystem<HUD>()->findGroupByName("Weapon 2 STAT " + to_string(i))->setHidden(false);
                    i++;
                }
            }
        } else {
            auto eitem2 = ztgk::game::scene->getChild("HUD")->getChild("Game")->getChild("Unit Details")->getChild("Weapon Portrait #2");
            eitem2->getComponent<Text>()->content = "*No Item*";
            eitem2->getComponent<Sprite>()->load("res/textures/question_mark.png");
            eitem2->getChild("Offensive Stats")->getChild("ATK")->getComponent<Sprite>()->load("res/textures/icons/stat/atk.png");
            ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w2_offensive)->setHidden(true);
            ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w2_passive)->setHidden(true);
        }
    
        if (!unit->equipment.item1 && !unit->equipment.item2) {
            auto eitem0 = ztgk::game::scene->getChild("HUD")->getChild("Game")->getChild("Unit Details")->getChild("Weapon Portrait #1");
            eitem0->getComponent<Text>()->content = unit->equipment.item0->name + " - No Item";
            eitem0->getComponent<Sprite>()->load(unit->equipment.item0->icon_path);
            if (unit->equipment.item0->offensive) {
                ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w1_offensive)->setHidden(false);
                ztgk::game::scene->systemManager.getSystem<HUD>()->getGroupOrDefault(ztgk::game::ui_data.gr_w1_passive)->setHidden(true);
    
                eitem0->getChild("Offensive Stats")->getChild("ATK")->getComponent<Text>()->content = std::format("{}",unit->equipment.item0->stats.dmg);
                eitem0->getChild("Offensive Stats")->getChild("RNG")->getComponent<Text>()->content = std::format("{}",unit->equipment.item0->stats.range.add);
                eitem0->getChild("Offensive Stats")->getChild("CD")->getChild("Display Bar")->getComponent<HUDSlider>()->displayMax = unit->equipment.item0->stats.cd_max_sec;
                eitem0->getChild("Offensive Stats")->getChild("CD")->getChild("Display Bar")->getComponent<HUDSlider>()->set_in_display_range(unit->equipment.item0->cd_sec);
            }
        }
    }
}