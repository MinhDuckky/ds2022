struct Request {
    string file<36>;
    string action<12>; 
};

program HANDLE_FILE {
    version PROG_VERS{
        string response(Request)=1;
    }=1;
}=0x77777777;