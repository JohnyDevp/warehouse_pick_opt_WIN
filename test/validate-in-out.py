import json
import sys

def final_validation():
    # check for arguments (json files)
    if len(sys.argv) < 3: exit("ERROR: 2 files as arguments are required!")

    # load json files from arguments
    original_file = open(sys.argv[1])
    final_file = open(sys.argv[2])
    of = json.load(original_file)
    ff = json.load(final_file)

    # count all {customer, item} doubles
    original_dict = {}
    num_of_original_items = 0
    num_of_only_address_diff = 0
    for item in of:
        # create a customer if it hasn't been registered yet
        if (item['Customer'] not in original_dict): 
            original_dict[item['Customer']] = []

        # append the product to its customer
        registered = False
        only_address_differs = False
        for product in original_dict[item['Customer']]:
            if item['Product'] == product['Product'] and item['AddressCode'] == product['AddressCode']:
                registered = True
                only_address_differs = False
            if item['Product'] == product['Product'] and item['AddressCode'] != product['AddressCode']:
                only_address_differs = True
        
        if only_address_differs:
            num_of_only_address_diff += 1

        # skip this item if already registered
        # otherwise enroll it
        if (not registered):
            original_dict[item['Customer']].append({'Product': item['Product'], 'Qty':item['Qty'], 'AddressCode':item['AddressCode']})
            num_of_original_items += 1
    
    num_of_original_units = 0
    for customer in original_dict.items():
        for item in customer[1]:
            num_of_original_units += item['Qty']

    print("***** ORIGINAL FILE *****")
    print("num of all doubles {Customer, Product}: " + str(num_of_original_items))
    print("num of all units: ", str(num_of_original_units))
    print()

    # get everything from the final json
    final_dict = {}
    num_of_final_items = 0
    total_qty = 0
    for cart in ff:
        # now i am in cart
        for item in cart['items']:
            total_qty += int(item['Qty'])
            # now going through all the items in cart
            # there are no doubles so add everything
            if (item['Customer'] not in final_dict):
                # create a customer if it doesn't exist yet
                final_dict[item['Customer']] = []
            
            # append the product for this customer
            registered = False
            for product in final_dict[item['Customer']]:
                if item['Product'] == product['Product'] and item['AddressCode'] == product['AddressCode']:
                    product['Qty'] = int(product['Qty']) + int(item['Qty'])
                    registered = True

            if not registered:
                final_dict[item['Customer']].append({'Product': item['Product'], 'Qty':item['Qty'], 'AddressCode':item['AddressCode']})
                num_of_final_items += 1          
    
    num_of_final_units = 0
    for customer in final_dict.items():
        for item in customer[1]:
            num_of_final_units += item['Qty']

    print("***** FINAL FILE *****")
    print("num of all doubles {Customer, Product}: " + str(num_of_final_items))
    print("num of all units: " + str(num_of_final_units))
    print("total qty: " + str(total_qty))
    print()

    # go through all the items, compare the original and final file and find possible differences
    differences_dict = {}
    num_of_differences = 0
    for orig_customer in original_dict.items():
        orig_cust_key = orig_customer[0]
        orig_cust_items = orig_customer[1]
        for item in orig_cust_items:
            # check whether all the items fits between the two files
            if orig_cust_key not in final_dict:
                if orig_cust_key not in differences_dict:
                    differences_dict[orig_cust_key] = []
                differences_dict[orig_cust_key].append(item)
                num_of_differences += 1
            elif item not in final_dict[orig_cust_key]:
                if orig_cust_key not in differences_dict:
                    differences_dict[orig_cust_key] = []
                differences_dict[orig_cust_key].append(item)
                num_of_differences += 1

    print("***** DIFFERENCES *****")
    print("number of items that differs: " + str(num_of_differences))
    print("all differences listed:")
    print(differences_dict)

def test_validation():
    # check for arguments (json files)
    if len(sys.argv) < 2: exit("ERROR: 2 files as arguments are required!")

    # load json files from arguments
    original_file = open(sys.argv[1])
    test_file = open(sys.argv[2])
    of = json.load(original_file)
    

    # count all {customer, item} doubles
    original_dict = {}
    num_of_original_items = 0
    num_of_only_address_diff = 0
    for item in of:
        # create a customer if it hasn't been registered yet
        if (item['Customer'] not in original_dict): 
            original_dict[item['Customer']] = []

        # append the product to its customer
        registered = False
        only_address_differs = False
        for product in original_dict[item['Customer']]:
            if item['Product'] == product['Product'] and item['AddressCode'] == product['AddressCode']:
                registered = True
                only_address_differs = False
            if item['Product'] == product['Product'] and item['AddressCode'] != product['AddressCode']:
                only_address_differs = True
        
        if only_address_differs:
            num_of_only_address_diff += 1
        # skip this item if already registered
        # otherwise enroll it
        if (not registered):
            original_dict[item['Customer']].append({'Product': item['Product'], 'Qty':item['Qty'], 'AddressCode':item['AddressCode']})
            num_of_original_items += 1
    
    num_of_original_units = 0
    for customer in original_dict.items():
        for item in customer[1]:
            num_of_original_units += item['Qty']

    print("***** ORIGINAL FILE *****")
    print("num of all doubles {Customer, Product}: " + str(num_of_original_items))
    print(" that differes only in Address: " + str(num_of_only_address_diff))
    print("num of all units: ", str(num_of_original_units))
    print()

   

if __name__ == "__main__":
    final_validation()
    
    
