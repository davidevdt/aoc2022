

file_name = "./input.txt"
packets = []
with open(file_name, "r") as f:
    for line in f: 
        packets.append(line.strip()); 
        
def compare_numbers(number_1, number_2): 
    if number_1 < number_2:
        return 1
    elif number_1 == number_2: 
        return 0 
    else:
        return -1

def compare_lists(list_a, list_b): 
    result = 0; 
    for (l1, l2) in zip(list_a, list_b): 
        if isinstance(l1, int) and isinstance(l2, int): 
            result =  compare_numbers(l1, l2)
            if (result != 0): 
                # print("different numbers: ", l1, " vs ", l2)
                return result
        elif (isinstance(l1, list) and isinstance(l2, int)):
            result = compare_lists(l1, [l2])
            if (result != 0): 
                # print("different lists (l1 is the list): ", l1, " vs ", l2)
                return result
        elif (isinstance(l1, int) and isinstance(l2, list)): 
            result = compare_lists([l1], l2)
            if (result != 0): 
                # print("different lists (l2 is the list): ", l1, " vs ", l2)
                return result
        elif (isinstance(l1, list) and isinstance(l2, list)): 
            result = compare_lists(l1, l2)
            if (result != 0): 
                # print("different lists (both lists): ", l1, " vs ", l2)
                return result
        
    if result != 0:
        return result; 

    if (len(list_a) < len(list_b)): 
        # print("list a smaller: ", list_a, " vs ", list_b)
        return 1
    elif (len(list_a) > len(list_b)):
        # print("list a bigger: ", list_a, " vs. ", list_b)
        return -1
    # print("equal lists", list_a, " vs. ", list_b)
    return 0 

i = 0
index_count = 0
results = 0 
while True:  
    index_count += 1
    p1 = eval(packets[i])
    p2 = eval(packets[i+1])
    # print(p1)
    # print(p2)
    
    r = compare_lists(p1, p2)
    # print("result:", r)
    # print("---")
    if (r > 0):
        results += (index_count)

    if  (i + 1) == len(packets) - 1: 
        break
    i += 3
    
print("The solution to part one is: ", results)