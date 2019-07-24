#include <getopt.h>

void get_args(
       int argc,
       char** argv,
       char** solace_host,
       char** solace_vpn,
       char** solace_user,
       char** solace_pwd,
       char** hb_host,
       char** hb_service_type,
       char** hb_instance,
       char** hb_version,
       char* hb_status,
       char** hb_comments,
       uint64_t* hb_start_time)
{
       static struct option long_options[] = 
               {
                       {"solace_host",         required_argument, 0, 'n'},
                       {"solace_vpn",          required_argument, 0, 'x'},
                       {"solace_user",         required_argument, 0, 'u'},
                       {"solace_pwd",          required_argument, 0, 'p'},
                       {"hb_host",         required_argument, 0, 'h'},
                       {"hb_service_type", required_argument, 0, 't'},
                       {"hb_instance",     required_argument, 0, 'i'},
                       {"hb_version",      required_argument, 0, 'v'},
                       {"hb_status",       required_argument, 0, 's'},
                       {"hb_comments",     required_argument, 0, 'c'},
                       {"hb_start_time",   required_argument, 0, 'a'},
                       {0, 0, 0, 0}
               };

       int option_index = 0;
       int c = getopt_long(argc, argv, "n:x:u:p:h:t:i:v:s:c:a:", long_options, &option_index);

       unsigned int status_temp;

       while ( c != -1 )
       {
               switch (c)
               {
                       case 'n':       *solace_host     = optarg;                              break;
                       case 'x':       *solace_vpn      = optarg;                              break;
                       case 'u':       *solace_user     = optarg;                              break;
                       case 'p':       *solace_pwd      = optarg;                              break;
                       case 'h':       *hb_host         = optarg;                              break;
                       case 't':       *hb_service_type = optarg;                              break;
                       case 'i':       *hb_instance     = optarg;                              break;
                       case 'v':       *hb_version      = optarg;                              break;
                       case 's':   sscanf(optarg, "%u",   &status_temp);          break;
                       case 'c':       *hb_comments     = optarg;                              break;
                       case 'a':       sscanf(optarg, "%llu", hb_start_time);  break;
                       default:
                               printf("Wrong ARGS!");
                               exit(1);
               }
               c = getopt_long(argc, argv, "n:x:u:p:h:t:i:v:s:c:a:", long_options, &option_index);
       }

       *hb_status = (char)status_temp;
}
