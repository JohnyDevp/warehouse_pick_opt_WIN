// #include "crow_all.h"
#include "include/crow.h"
#include "WarehousePickingOpt.h"
#include "warehouse_picking_opt_debug.h"
#include <getopt.h>

int main(int argc, char *argv[])
{
    // port is by default 8080
    int port = 8080;
    short bp_solver = 1, grouping_alg = 0, tsp_solver = 2;
    for (;;)
    {
        switch (getopt(argc, argv, "p:b:g:s:")) // note the colon (:) to indicate that 'b' has a parameter and is not a switch
        {
            case 'p':
                port = atoi(optarg);
                cerr << "port set to " << port << endl;
                continue;

            case 'b':
                bp_solver = atoi(optarg);
                cerr << "bin packing solver set to " << bp_solver << endl;
                continue;

            case 'g':
                grouping_alg = atoi(optarg);
                cerr << "grouping customers algorithm set to " << grouping_alg << endl;
                continue;
            case 's':
                tsp_solver = atoi(optarg);
                continue;
            default:
                cerr << "-p" << "   " << "set the port" << endl;
                break;

            case -1:
                break;
        }
        break;
    }

    // print the setup
    cerr << "port set to " << port << endl;
    cerr << "bin packing solver set to " << bp_solver << endl;
    cerr << "grouping customers algorithm set to " << grouping_alg << endl;
    cerr << "tsp solver set to " << tsp_solver << endl;

    is_debug_on = true;

    SolverConfiguration sc = SolverConfiguration(
            (BPAlgorithms)bp_solver,
            (GroupingCustomersAlgorithms)grouping_alg,
            (ShortestPathAlgorithms)tsp_solver);
        
    crow::SimpleApp app;

    CROW_ROUTE(app, "/input_data").methods("POST"_method)([&sc](const crow::request &req)
    {
        try{
            // build the configurations --------------------------------------------------
            // pri dvou stejnych rozmerech a pouze jednom jinem ztraci svou vyhodu FFDSEnhance 3D algoritmus oproti knihovnimu algoritmu
            BoxConfiguration bc;
            bc.addBox(Box(28.0, 29.0, 45.0, 4000, "masterBox"), 1000); // the original and so far REAL box
            CartConfiguration cc = CartConfiguration(90, 200, 50, 1, 3);
            Warehouse w;
            CartsLoader CL(w);
            //----------------------------------------------------------------------------------------------------

            CL.loadItemsAndSpec(req.body);
            CL.loadingCartsRun(sc, cc, bc);
            
            return CL.exportCartsToJson(false); 
        } catch (...) {
            crow::response(crow::status::BAD_REQUEST);
            return string("FAILURE");
        } });
    
    app.port(port).run();
}