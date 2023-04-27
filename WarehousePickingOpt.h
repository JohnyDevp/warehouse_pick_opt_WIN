#ifndef _WAREHOUSE_PICKING_OPTIMIZATION_H_
#define _WAREHOUSE_PICKING_OPTIMIZATION_H_

#include <string>
#include <list>
#include <vector>
#include <map>
#include <math.h>

using namespace std;

// incomplete declarations ===============
class Box;
class Cart;
class CartShelf;
class OItem;
class BoxItem;
class Customer;

// class SolverConfiguration;
// class CartConfiguration;
// class BoxConfiguration;

//================================NOT SURE / setup_configuration.h ====================================================
enum BPAlgorithms
{
    FFD_1D_ORDER_ITEMS_NO_SPLIT,
    FFD_1D_ORDER_ITEMS_SPLIT_ONE_BOX_TYPE,
    FFDS_1D_ORDER_ITEMS_SPLIT_MULTI_BOX_TYPE,
    PYLIB_3D_ORDER_ITEMS_SPLIT_ONE_BOX_TYPE,
    PYLIB_3D_FFDSEnhance_ORDER_ITEMS_SPLIT_MULTI_BOX_TYPE,
    PYLIB_3D_ORDER_ITEMS_SPLIT_MULTI_BOX_TYPE_COUNT_DEFINED
};

enum GroupingCustomersAlgorithms
{
    AVG_DISTANCES,
    LEAST_SQUARE_APPROXIMATION,
    K_MEANS_CLUSTERING,
    AUTO
};

enum ShortestPathAlgorithms
{
    NEAREST_NEIGHBOUR,
    NEAREST_NEIGHBOUR_BRUTE_FORCE_OPTIMIZATION,
    GREEDY,
    GREEDY_BRUTE_FORCE_OPTIMIZATION,
    JUST_MEASURE_DISTANCE,
    NONE
};

/// @brief object representing the setup for problem solving
class SolverConfiguration
{
public:
    /// @brief constructor, when no params set the default are used
    /// @param _bp_alg chosen algorithm for bin packing
    /// @param _grouping_customers_alg chosen algorithm for grouping customers
    /// @param _shortest_path_alg chosen algorithm for sorting the items in cart to shortest path
    SolverConfiguration(
        BPAlgorithms _bp_alg = BPAlgorithms::FFD_1D_ORDER_ITEMS_NO_SPLIT,
        GroupingCustomersAlgorithms _grouping_customers_alg = GroupingCustomersAlgorithms::AVG_DISTANCES,
        ShortestPathAlgorithms _shortest_path_alg = ShortestPathAlgorithms::NEAREST_NEIGHBOUR);

    // when selected K-means clustering, this is maximal number of iterations
    int Number_of_K_Means_Iterations = 40;
    // when selected K-means clustering, this is the optimal koeficient for multiplying portion (volume of wave)/(volume of cart)
    float percentage_kmeans = 0.91;

    BPAlgorithms getBPAlgorithm();
    GroupingCustomersAlgorithms getGroupingCustomerAlgorithm();
    ShortestPathAlgorithms getShortestPathAlgorithm();

private:
    BPAlgorithms bp_algorithm;
    GroupingCustomersAlgorithms grouping_customers_algorithm;
    ShortestPathAlgorithms shortest_path_algorithm;
};

/// @brief the configuration of the boxes, which will be used
class BoxConfiguration
{
private:
    /// @brief vector of pairs consisting of box_type and count of it
    vector<pair<Box, int>> box_types_count_vect;
    vector<Box> box_types_vect;

public:
    /// @brief add a box and its count
    /// @param b box instance
    /// @param count number representing the occurences of this box type
    /// in a warehouse, set to -1 when count its infinity
    void addBox(Box b, long count = -1);

    vector<pair<Box, int>> getBoxesWCountVector();

    vector<Box> getBoxesVector();
    /// @brief get the first box in the box vect
    /// @return first box in box vector or empty box instance if there is no box
    Box getFirstBox();
};

class CartConfiguration
{
private:
    /// @brief the cart pattern - include only
    vector<Cart> cart_pattern;

    /// @brief  vector of shelves in the cart
    vector<CartShelf> cart_shelf_vect;

    float cart_width = 0;
    float cart_height = 0;
    float cart_depth = 0;

    // TODO not used...
    float usable_capacity = 0;

    int number_of_shelves = 0;

public:
    /// @brief constructor for a cart pattern, this pattern will be used while creating new carts,
    /// paramteres like depth and width will be inhereted by cart shelves \n
    /// @warning be sure that it will be compatible with the box configuration, because there is slight simplification \n
    /// when called, there will be automaticcaly constructed cart with shelves according to the number, each of height in particular proportion
    /// @param _cart_width the usable width of the cart (the number must exclude all frames!)
    /// @param _cart_height the usable height of the cart (the number must exclude all frames!)
    /// @param _cart_depth the usable depth of the cart (the number must exclude all frames!)
    /// @param _usable_capacity the usable capacity of the whole computed capacity (volume) of the cart,
    /// will be inhereted by the shelves
    CartConfiguration(float _cart_width, float _cart_height, float _cart_depth, float _usable_capacity, int _number_of_shelves);

    /// @brief get the created cart pattern
    /// @return created cart pattern to be used while creating carts used in algorithm
    Cart getCartPattern();

    /// @brief removed all shelves from cart
    /// @warning when no else shelf created, the algorithm won't work
    void removeCartShelves();

    /// @brief add shelf of given parameters
    /// @warning the parameters can overflow the cart paramterers and it won't be noticed!!!!
    void addCartShelf(float _shelf_width, float _shelf_height, float _shelf_depth, float _usable_capacity);
};

//================================NOT SURE / waddress.h ====================================================
/// @brief this structure stores parsed address in usable form in computation
struct SECTOR_ADDR_T
{
    string section{};         // G00,V02,GP1,P00,... (may vary depending on usage)
    int y_coord = 0;          // floor row[Y-axis]
    int z_coord = 0;          // regal_floor(00-04/15)[Z-axis]
    int x_coord = 0;          // cell[X-axis]
    int x_coord_original = 0; // original x coord right from the address
    int y_coord_original = 0; // original y coord right from the address
    int z_coord_original = 0; // original z coord right from the address

    inline bool operator==(const SECTOR_ADDR_T &lhs)
    {
        return (lhs.x_coord == x_coord && lhs.y_coord == y_coord);
    }
};

/// @brief class representing the WAddress of product in warehouse
class WAddress
{
public:
    /// @brief constructor with param of raw address which is immediately parsed to proper form
    /// @param raw_sector_addr the address of item in sector in string format
    WAddress(string raw_sector_addr);

    /// @brief Destroy the WAddress object
    ~WAddress();

    /// @brief Get the real Warehouse WAddress
    /// @return SECTOR_ADDR_T the real WAddress in warehouse
    SECTOR_ADDR_T getWarehouseWAddress();

    /// @brief getter
    /// @return raw address in sector in string
    string getRawSectorAddress();

private:
    /// @brief stored real warehouse WAddress of some place
    SECTOR_ADDR_T parsed_sector_addr;

    /// @brief raw sector addr as string
    string raw_sector_addr;
};

//================================NOT SURE / warehouse.h ===================================================
/// @brief warehouse representation, distance computation
class Warehouse
{
private:
    /// @brief used as max x coordinate
    int width_x;

    /// @brief used as max y coordinate
    int height_y;

public:
    /// @brief constructor
    Warehouse();
    ~Warehouse();

    /// @brief function counting distance between two given addresses
    /// @return distance between two addresses
    int getDistanceBetweenAddresses(WAddress first_addr, WAddress second_addr);

    /// @brief function counting distance between two given addresses
    /// @warning both addresses have to have set up all values in struct, otherwise the result may be inappropriate
    /// @param first_addr
    /// @param second_addr
    /// @return distance between two addresses according to the warehouse layout
    int getDistanceBetweenAddresses(SECTOR_ADDR_T first_addr, SECTOR_ADDR_T second_addr);
};

//============================== NOT SURE / general_declarations.h==========================================
/// @brief
class ItemSpecification
{
public:
    /// @brief enum for item specification type (can be either Unit or Car as whole package of items)
    enum class Type
    {
        Unit,
        Carton,
        Pallet
    };

    /// @brief constructor
    /// @param Product product number
    /// @param Unit which kind of package do the data belong to
    /// @param PackageType
    /// @param ContentQty
    /// @param X
    /// @param Y
    /// @param Z
    /// @param Weight
    ItemSpecification(int _Product, ItemSpecification::Type _Unit, int _PackageType, int _ContentQty, float _X, float _Y, float _Z, float _Weight);
    ItemSpecification(){};
    ~ItemSpecification();

    int Product;
    ItemSpecification::Type Unit;
    int PackageType;
    int ContentQty;
    float X;
    float Y;
    float Z;
    float Weight;
    float Volume;
};

/// @brief class representing the item in order (with its address, and all description) -> Order Item
class OItem
{
private:
    /// @brief total weight of this item
    float weight;
    /// @brief total volume of this item
    float volume;

public:
    int Customer;
    int Product;
    string Address; // the address as used in real warehouse
    int Qty;
    /// @brief address in its representation for algorithm proposes
    WAddress *waddress;

    /// @brief item specification reference
    map<ItemSpecification::Type, ItemSpecification> item_spec;

    /// @brief constructor
    /// @param Customer Customer number - used for recognition which items belong to one customer
    /// @param Product Product number - used for recognition which item specification belongs to which item
    /// @param Address Address code string - the string, which contains exact position of item in warehouse in 3D coordinates
    /// @param Qty Quantity of units in this item
    OItem(int Customer, int Product, string AddressCode, int Qty);
    OItem() = default;
    ~OItem();

    /// @brief adding the item specification
    void addItemSpecification(ItemSpecification itspec) { this->item_spec.insert({itspec.Unit, itspec}); }

    /// @brief getter of item weight
    /// @return weight of item as float - ALL OF THE UNITS INCLUDED
    float getWeight();
    /// @brief  getter of item volume
    /// @return volume of item as float - ALL OF THE UNITS INCLUDED
    float getVolume();

    /// @brief function for counting the volume and weight of this item (because this item can include more units)
    void countWeightAndVolume();
};

/// @brief class which serves as representation of item in the box
/// in difference from OItem it doesn't represent order item of N units - instead of this BoxItem represents only
/// one unit of OItem or a package of N units of OItem
class BoxItem
{
private:
public:
    /// @brief the count of UNITS of this item in the box (even if this item represents package, this variable stores the count OF UNITS!!!!)
    /// e.g. 6 phones will be stored as one BoxItem of count=6 or 2 packs of phones will be stored as one BoxItem of count=2
    int count_of_units = 0;
    ItemSpecification item_specification;
    OItem order_item;

    BoxItem(ItemSpecification _item_specification, OItem _order_item);
    BoxItem() = default;

    /// @brief increase the number stored in count variable
    void countUnitsUp(int num);
    void setNumOfUnits(int num);
    int getCountOfUnits();

    int getVolume();
};

/// @brief structure
typedef struct items_into_boxes_decomposition
{
    vector<Box> full_box_vect;
    vector<Box> one_item_box_vect;
} ITEMS_IN_BOXES_DECOMP_T;

/// @brief the shelf in cart, which contains boxes
class CartShelf
{
private:
    float used_width = 0;

    ITEMS_IN_BOXES_DECOMP_T boxes;

    float width;
    float height;
    float depth;

public:
    CartShelf(float width, float height = 99999999.9, float depth = 99999999.9);

    void addBox_oneItemized(Box box);

    void addBox_fullItemized(Box box);

    ITEMS_IN_BOXES_DECOMP_T *getBoxes();

    float getWidth();
    float getHeight();
    float getDepth();
    float getFreeWidth();
};

/// @brief class representing the customer object (containing its items in warehouse - in order)
class Customer
{
private:
    float total_weight = 0.0f;
    float total_volume = 0.0f;

    /// @brief id of customer loaded from one of its items
    /// the -1 signs, that it has no item inside yet (after first gets loaded this variable will change)
    int customer_id = -1;

    /// @brief the decomposition of items into boxes in standart variant of cart (9 boxes of constant volume)
    vector<Box> box_vect;

public:
    /// @brief all items that belongs to the customer
    vector<OItem> item_vect;

    ~Customer();

    Customer(int cust_id) : customer_id(cust_id){};

    /// @brief add item to this customer
    void addOItem(OItem o_item);

    /// @brief function which counts volume and weight of all items
    void countTotalWeightAndVolume();

    void decompIntoBoxes(BoxConfiguration &bc, SolverConfiguration &sc);

    vector<Box> getBoxesDecomp();

    void setBoxesDecomp(vector<Box> _box_vect);

    float getTotalWeight();
    float getTotalVolume();
    int getCustomerID();
};

/// @brief class representing part or whole of order item in cart, differ from OItem only by
/// added RealQty integer member, which gives an information of how many pieces of order item is realy in the cart
class CartOItem
{
private:
    /// @brief gives an information of how many pieces of order item is realy in the cart
    OItem oitem_ref;

public:
    int RealQty = 0;
    CartOItem(OItem &_oitem_ref);
    void addToRealQty(int qty_to_add);
    int getRealQty();

    int order = 1;
    OItem getOItem() const;
};

/// @brief class representing one Cart loaded with items from customers
class Cart
{
private:
    /// @brief list of loaded customers
    vector<Customer> customer_vect;

    /// @brief vector with all items with its count of units in cart - finally it is SORTED to the best shortest path(TSP problem)
    vector<OItem> item_vect; // TODO remove
    vector<CartOItem> cartoitem_vect;
    void addOItemWithQty(OItem oitem, int qty);

    /// @brief vector
    vector<CartShelf> cart_shelves_vect;

    /// @brief whole free space in cart (all boxes's free space together)
    float cart_free_space;
    /// @deprecated not used for real anymore
    int free_boxes = 9;

    /// @brief the currently estimated walked distance between items assigned to this cart
    int estimated_walked_distance = 0;

    bool loadingCustomer_old(Customer &customer);
    bool loadingCustomer_new(SolverConfiguration &sc, Customer &customer, bool can_be_partly_loaded = false, vector<Box> *non_loaded_boxes = NULL);

public:
    Cart();
    /// @brief not in use but ready //TODO
    /// @param cart_shelf_vect
    Cart(vector<CartShelf> cart_shelf_vect);

    ~Cart();

    /// @brief function for attaching customer to some cart
    /// @param customer the customer to be loaded
    /// @return boolean according to the whether the customer fit the free space in cart or not
    bool loadCustomer(SolverConfiguration &sc, Customer &customer, bool can_be_partly_loaded = false, vector<Box> *non_loaded_boxes = NULL);

    /// @brief no check, the customer is load by force, even if it doesn't fit
    /// @attention it is recomended to firstly try loadCustomer to get real-view
    void forceLoadCustomer(Customer &customer);

    /// @brief getter for whole free space on cart
    /// @return
    float getCartFreeSpace();

    /// @brief getter for the built customer list
    /// @return list of customers
    vector<Customer> getCustomerVector();

    /// @brief
    /// @return vector with items in cart
    vector<OItem> getItemVector();
    vector<CartOItem> getCartOItemVector();

    vector<CartShelf> getCartShelvesVector();
    /// @brief setter
    void setCartOItemVector(vector<CartOItem> _cartoitem_vect);

    /// @brief function for counting the distance which is any person supposed to walk when handling this cart
    /// @param dist distance to be counted
    void addToEstimatedWalkedDistance(int dist);

    int getEstimatedWalkedDistance();
};

//===================== SHOULD BE PUBLIC ======================================================
/// @brief Box that can be loaded to the cart and which contains picked items in the warehouse
class Box
{
private:
    /// @brief this is computed according to the parameters of the box
    /// it is computed as width*height*depth*usable_capacity_percentage
    float capacity = 0;

    /// @brief capacity of box still free (not used by any items)
    float free_capacity = 0;

    /// @brief capacity of box used already by items
    float used_capacity = 0;

    /// @brief vector of items which are not neccessary all OItems
    multimap<int, BoxItem> product_boxitem_map;

public:
    /// @brief usable capacity in percentage
    float width, height, depth, max_weight;
    string name;
    float usable_capacity_percentage = 1;

    /// @brief constructor, parameters are considered looking from the front side of the box
    /// @param width width of the box (from the front side)
    /// @param height height of the box (from the front side)
    /// @param depth depth of the box (from the front side - the length from the front side to the back-side) \n
    /// @param max_weight maximal weight which can be loaded to the box
    /// @param name optional name of the box
    Box(float width, float height, float depth, float max_weight, string name, float _usable_capacity_percentage = 1);
    Box() = default;
    ~Box();

    /// @return capacity of box
    float getCapacity();

    /// @return free capacity of the box (this capacity is not necessarily usable in reality (e.g. some tight free space along item,...))
    float getFreeCapacity();

    /// @return capacity used by the items already in the box
    float getUsedCapacity();

    /// @return map of <Product,BoxItems> that represents Product's units in the box
    multimap<int, BoxItem> getItemsInTheBox();

    /// @brief function for adding item to box
    /// @warning it is without any check
    bool addItem(BoxItem &boxItem);

    /// @brief function returning the empty state of the box
    /// @return true when empty or false when not-empty
    bool isEmpty();
};

/// @brief main class representing something like an interface for it
class CartsLoader
{
private:
    /// @brief object describing warehouse
    Warehouse warehouse;

    /// @brief vector with all items passed to this algorithm (each is individual part)
    vector<OItem> item_vect;

    /// @brief vector with customers, that gather items to some undivideable groups
    map<int, Customer> id_customer_map;

    /// @brief list with finally loaded carts
    list<Cart> loaded_cart_list;

    /// @brief computes all implemented versions and choose the best
    /// @param solver_conf
    /// @param cart_conf
    /// @param volume_customer_map
    /// @return the right algorithm
    short chooseBestAlgorithm(
        SolverConfiguration &solver_conf,
        CartConfiguration &cart_conf,
        multimap<int, Customer, greater<int>> volume_customer_map);

public:
    CartsLoader(Warehouse &warehouse);
    ~CartsLoader();

    /// @brief function for loading up all items given in order with their specification
    void loadItemsAndSpec(string path_or_text);

    /// @brief start algorithm
    void loadingCartsRun(SolverConfiguration &solver_conf, CartConfiguration &cart_conf, BoxConfiguration &box_conf);

    /// @brief export created carts to json file, which is created in the given or root directory
    string exportCartsToJson(bool export_shelves_boxes = true, bool export_sorted_items = true);
    void testUsedData(string path);
    string getStats();
    list<Cart> getLoadedCartsList();

    // FIXME for testing
    int getWholeDistance()
    {
        int dst = 0;
        for (auto cart : this->loaded_cart_list)
            dst += cart.getEstimatedWalkedDistance();
        return dst;
    };
    int getAvgOfOnePiecedCustomersToCart()
    {
        int num = 0;
        int max = 0;
        for (auto cart : this->loaded_cart_list)
            for (auto shelf : cart.getCartShelvesVector())
            {
                int tmp = 0;
                for (auto box : shelf.getBoxes()->one_item_box_vect)
                {
                    num += box.getItemsInTheBox().size();
                    tmp += box.getItemsInTheBox().size();
                }
                if (tmp > max)
                    max = tmp;
            }
        return (int)(num / this->loaded_cart_list.size());
    }
};

//===============================================================================================
#endif