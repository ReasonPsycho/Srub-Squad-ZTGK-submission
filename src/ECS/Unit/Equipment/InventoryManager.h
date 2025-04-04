//
// Created by cheily on 06.05.2024.
//

#pragma once

#include "ECS/System.h"
#include "ECS/Unit/Equipment/Item.h"

// Items are not components but this should still be on the scene graph somewhere.
class InventoryManager : public Component {
public:
    static InventoryManager * instance;

    InventoryManager();

    std::vector<std::unique_ptr<Item>> free_items;
    std::vector<std::unique_ptr<Item>> assigned_items;

    std::unordered_map<unsigned, std::function<Item*()>> item_constructors;

    void init();

    Item * create_item(unsigned type_id);
    Item * create_default_item(Unit * unit);
    // CAREFUL! This doesn't unequip the item! Use unassign_and_delete_item instead or call unassign_item before!
    void delete_item(unsigned item_id);
    void delete_item(Item * item);

    // nullptr if not found, bool true means the item is assigned
    std::pair<Item *, bool> get_item(unsigned item_uid);

    // item cannot be assigned if it's not free!
    std::pair<Item *, Item *> assign_item(Item * item, Unit * unit, short slot);
    bool unassign_item(Unit * unit, Item * item);
    bool unassign_item(Unit * unit, short slot);

    bool create_and_assign_item(unsigned type_id, Unit * unit, short slot);
    bool unassign_and_delete_item(Unit * unit, short slot);
    bool unassign_and_delete_item(Unit * unit, Item * item);

//    void spawn_item_on_map(Item * item, glm::vec2 world_pos);
    void spawn_item_on_map(Item * item, Vector2Int grid_pos);

    void showImGuiDetailsImpl(Camera *camera) override;

private:
    bool mv_to_free(Item * item);
    bool mv_to_asgn(Item * item);
};
