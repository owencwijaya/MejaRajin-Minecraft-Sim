#include "headers/table.hpp"
#include "../globals.hpp"

namespace Lib {

    // IMPLEMENTATION FIELD FOR CHILD CLASS: Inventory
    /**
     * @brief Default Constructor for Class Inventory
     *  Initializes the inventory with a default size of INV ROWS x INV COLS slots
     *  Each slot (buffer) is initialized with a default Item Constructor
     *
     *
     */
    Inventory::Inventory() : Table(INV_ROWS, INV_COLS){}

    /**
     * @brief Overload the << operator to display the Inventory
     * 
     * @param os Output Stream
     * @param inven inventory to be displayed
     * @return ostream& reference to the output stream
     */
    ostream& operator<<(ostream& os, Inventory& inven) {
        os << "\nInventory: " << endl;
        for (int i = 0; i < INV_SIZE; i++) {
            os << "[(I" << i << ") "
                << (inven.slot[i])->getID() << " "
                << (inven.slot[i]->isTool() ?
                    inven.slot[i]->getDurability() :
                    inven.slot[i]->getQuantity()) << "] ";
            if ((i + 1) % INV_COLS == 0) {
                os << endl;
            }
        }
        return os;
    }

    /**
     * @brief Overload the << operator to display inventory details
     * 
     * @param os Output Stream
     * @param inven inventory to be displayed details
     * @return ostream& reference to the output stream
     */
    ostream& operator<<(ostream& os, Inventory* inven) {
        int undef_count = INV_SIZE;
        os << "\n\n[INVENTORY DETAILS]" << endl;
        os << "Slot" << " | "
            << setw(NUMWIDTH) << "ID" << " | "
            << setw(WIDTH) << "Name" << " | "
            << setw(WIDTH) << "Type" << " | "
            << setw(WIDTH) << "Base Type" << endl;
        for (int i = 0; i < INV_SIZE; i++) {
            if (inven->slot[i]->getID() != UNDEFINED_ID) {
                undef_count--;
                os << setw(NUMWIDTH - to_string(i).length()) << "I" << i << " | ";
                inven->specify(i);
                os << endl;
            }
        }
        if (undef_count == INV_SIZE){
            os << "\nNo items in inventory :(" << endl;
        }
        return os;
    }

    /**
     * @brief Returns the number of filled slots in the inventory
     *
     * @return The number of filled slots
     */
    int Inventory::filledSlots() {
        int count = 0;
        while (this->get(count)->getID() != UNDEFINED_ID) {
            count++;
        }
        return count;
    }

    /**
     * @brief Adds a non-tool item to the inventory
     *
     * @param item item to be added
     * @param start start index to begin adding
     */

    void Inventory::addNonTool(NonTool* item, int start){
        Item* currItem = nullptr;
        for (int i = start; i < INV_SIZE; i++) {
            currItem = this->get(i);
            // case 1: other item exists (stackable)
            if (currItem->getID() == item->getID()) {
                // if current slot less than max stack, increase quantity
                if (currItem->getQuantity() + item->getQuantity() <= MAX_STACK) {
                    this->get(i)->setQuantity(this->get(i)->getQuantity() + item->getQuantity());
                    return;
                }
                else {
                    item->setQuantity(item->getQuantity() - (MAX_STACK - this->get(i)->getQuantity()));
                    currItem->setQuantity(MAX_STACK);
                    this->addNonTool(item, i + 1);
                    return;
                }
            }
        }
        // case 2: if not found, find from the first slot
        for (int i = 0; i < INV_SIZE; i++){
            currItem = this->get(i);
            if (currItem->getID() == UNDEFINED_ID) {
                if (item->getQuantity() <= MAX_STACK) {
                    this->set(i, item);
                    return;
                }
                else {
                    NonTool* NT = new NonTool(*item);
                    NT->setQuantity(MAX_STACK);
                    this->set(i, NT);
                    item->setQuantity(item->getQuantity() - MAX_STACK);
                    continue;
                }
            }
        }
        return;
    }

    /**
     * @brief Adds a tool item to the inventory
     *
     * @param item item to be added
     * @param quant quantity of the item
     */
    void Inventory::addTool(Tool* item, int qty){
        bool added = false;
        Tool* temp;
        for (int i = 0; i < qty; i++) {
            temp = new Tool(*item);
            added = false;
            for (int j = 0; j < INV_SIZE; j++) {
                // base case if no such item exists in inventory
                if (this->get(j)->getID() == UNDEFINED_ID) {
                    this->set(j, temp);
                    added = true;
                    break;
                }
            }
            if (!added) {
                throw new InvException("FULL");
                delete temp;
            }
        }
    }

     /**
     * @brief Discards an item from a slot in the inventory
     *
     * @param index the destination index to be discarded
     * @param qty quantity of the item
     */

    void Inventory::discard(int index, int qty) {
        Item* target = this->slot[index];
        if (target->getQuantity() - qty > 0) {
            target->setQuantity(target->getQuantity() - qty);
        }
        else if (target->getQuantity() - qty == 0) {
            set(index, new Item());
        }
        else {
            throw new InvException("OVER");
        }
    }

    void Inventory::toCraft(int slotSrc,int destSlot[], int N){
        Item* undef_item = new Item();
        Item* item_inv = nullptr;
        Item* item_craft = nullptr;
        Item* item_moved = nullptr;
        Item* itInv = gm.inv[slotSrc];
        bool tool = itInv->isTool();

        if (tool) {
            item_inv = new Tool(*((Tool*)itInv));
        }
        else {
            item_inv = new NonTool(*((NonTool*)itInv));
        }

        if (item_inv->getID() == UNDEFINED_ID) {
            throw new MoveException("VOID");
        }
        else {
            if (!tool) {
                if (N > item_inv->getQuantity()) {
                    throw new MoveException("NOTENOUGH");
                }
                else {
                    item_moved = new NonTool(*((NonTool*)itInv));
                    item_moved->setQuantity(1);

                    for (int i = 0; i < N; i++) {
                        Item* itCrf = gm.crftab[destSlot[i]];
                        item_moved = new NonTool(*((NonTool*)itInv));
                        item_moved->setQuantity(1);
                        item_craft = new NonTool(*((NonTool*)itCrf));
                        if (item_craft->getID() != UNDEFINED_ID && item_craft->getID() != item_moved->getID()) {
                            throw new MoveException("DIFFTYPE");
                        }
                        else if (item_craft->getID() == UNDEFINED_ID) {
                            gm.crftab.set(destSlot[i], item_moved);
                        }
                        else {
                            itCrf->setQuantity(itCrf->getQuantity() + 1);
                        }
                    }
                    if (item_inv->getQuantity() - N == 0) {
                        gm.inv.set(slotSrc, undef_item);
                    }
                    else {
                        item_inv->setQuantity(item_inv->getQuantity() - N);
                        gm.inv.set(slotSrc, item_inv);
                    }
                }
            }
            else {
                Item* item_craft = new Item(*gm.crftab[destSlot[0]]);
                if (item_craft->getID() != UNDEFINED_ID) {
                    throw new MoveException("TOOL");
                }
                else {
                    Tool* item_moved = new Tool(*((Tool*)gm.inv.get(slotSrc)));
                    gm.inv.set(slotSrc, undef_item);
                    gm.crftab.set(destSlot[0], item_moved);
                }
            }
        }
    }

    void Inventory::toAnotherSlot(int slotSrc,int destSlot[]){
        Item* undef_item = new Item();
        Item* item_inv = nullptr;
        Item* item_craft = nullptr;
        Item* item_moved = nullptr;
        Item* item_inv2 = nullptr;
        item_inv2 = new Item(*gm.inv[destSlot[0]]);
        bool destKosong = true;
        Item* itInv = gm.inv[slotSrc];
        bool tool = gm.inv[slotSrc]->isTool();
        if (tool) {
            item_inv = new Tool(*((Tool*)itInv));
        }
        else {
            item_inv = new NonTool(*((NonTool*)itInv));
        }
        if (item_inv->getID() == UNDEFINED_ID) {
            throw new MoveException("VOID");
        }
        if (item_inv2->getID() != UNDEFINED_ID) {
            destKosong = false;
        }
        if (!destKosong && item_inv2->getID() != item_inv->getID()) {
            throw new MoveException("DIFFTYPE");
        }
        if (destKosong) {
            gm.inv.set(destSlot[0], item_inv);
            gm.inv.set(slotSrc, undef_item);
        }
        else {
            if (!tool) {
                Item* itInv2 = gm.inv[destSlot[0]];
                item_inv2 = new NonTool(*((NonTool*)itInv2));
                int kurang = 64 - item_inv2->getQuantity();
                if (item_inv->getQuantity() > kurang) {
                    item_inv2->setQuantity(MAX_STACK);
                    item_inv->setQuantity(item_inv->getQuantity() - kurang);
                    gm.inv.set(slotSrc, item_inv);
                    gm.inv.set(destSlot[0], item_inv2);
                }
                else {
                    item_inv2->setQuantity(item_inv2->getQuantity() + item_inv->getQuantity());
                    gm.inv.set(slotSrc, undef_item);
                    gm.inv.set(destSlot[0], item_inv2);
                }
            }
            else {
                throw new MoveException("TOOL");
            }

        }
    }        
}