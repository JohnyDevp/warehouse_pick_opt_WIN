## Parameters
`-p` for set up port number 
- defaultly on 8080

`-b` for set up the algorithm for packing boxes

- defaultly to 1 
- 0 ... for non-dividing items, just putting order items into box
- 1 ... 1D algorithm, splitting items into cartons and units, more accurate
- 2 ... 1D algorithm, splitting items into cartons and units, able to use multiple boxes

`-g` for set up the algorithm for grouping customers 
to carts
- defaultly to 0 
- 0 ... method using average distances between items, best according to tests
- 1 ... least square approximation, not so good
- 2 ... k means clustering, better in some cases
- 3 ... auto choosing the best of three method mentioned above

`-s` for set up the algorithm for solving shortest path between items in carts
- defaultly to 2
- 0 ... nearest neighbor, very easy
- 1 ... nearest neighbor with brute force optimization for carts with up to 10 items
- 2 ... greedy algorithm, the best here
- 3 ... greedy algorithm with brute force optimization for carts with up to 10 items


none of the parameters listed is necessary to use

## Run
1. to start the api run `./wpo_api.exe` (if necessary, allow firewall)
2. send raw (like string) data to **http://0.0.0.0:PORT/input_data**, where PORT stands for the number set for it (defaultly 8080) with method **POST**
3. wait some time (maximum measured reached 30 second) and obtain **JSON-formated result**

## Recomendation
For testing use POSTMAN program, available for Windows from https://www.postman.com/downloads/.
