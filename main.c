#include <stdio.h>
#include <stdlib.h>
#include <cups/cups.h>

typedef struct{
	int num_dests;
	cups_dest_t *dests;
} my_user_data_t;

cups_dest_t *ds;

int my_dest_cb_v1(my_user_data_t *user_data, unsigned flags, cups_dest_t *dest){
  
  if (flags & CUPS_DEST_FLAGS_REMOVED){
    //Remove destination from array...
    user_data->num_dests = cupsRemoveDest(dest->name, dest->instance, user_data->num_dests, &(user_data->dests));
  }else{
	// add destination to array...
    user_data->num_dests = cupsCopyDest(dest, user_data->num_dests, &(user_data->dests));
  }

  return (1);
}

int my_get_dests_v1(cups_ptype_t type, cups_ptype_t mask, cups_dest_t **dests){
  my_user_data_t user_data = { 0, NULL };

  if (!cupsEnumDests(CUPS_DEST_FLAGS_NONE, 1000, NULL, type,
                     mask, (cups_dest_cb_t)my_dest_cb_v1,
                     &user_data)){
   /*
    * An error occurred, free all of the destinations and
    * return...
    */

    cupsFreeDests(user_data.num_dests, user_data.dests);

    *dests = NULL;

    return (0);
  }

 /*
  * Return the destination array...
  */

  *dests = user_data.dests;

  return (user_data.num_dests);
}


void example_enum_dests_v1(){
	const int num_dests = my_get_dests_v1(0, 0, &ds);
  	printf("Найдено %d\n", num_dests);
    for (int k=0; k<num_dests; k++){
     	
     	const char *model = cupsGetOption("printer-make-and-model",
                                  ds[k].num_options,
                                  ds[k].options);
     	printf("    %s (%s)\n", ds[k].name, model);
    }
}





int print_dest_v0(void *user_data, unsigned flags, cups_dest_t *dest){
  if (dest->instance)
    printf("- %s/%s\n", dest->name, dest->instance);
  else
	printf("- %s\n", dest->name);

  return (1);
}

void example_enum_dests_v0(){
	cupsEnumDests(CUPS_DEST_FLAGS_NONE, 1000, NULL, 0, 0, print_dest_v0, NULL);	
}

int main (int argc, char **argv) {
  //example_enum_dests_v0();
	example_enum_dests_v1();
  return (0);
}


