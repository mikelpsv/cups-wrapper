#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <cups/cups.h>



typedef struct{
	int num_dests;
	cups_dest_t *dests;
} my_user_data_t;

cups_dest_t *ds;


void printf_pdf(cups_dest_t *dest){
	int job_id = 0;
	int num_options = 0;
	cups_dinfo_t *info;
	cups_option_t *options = NULL;
	FILE *fp = fopen("filename.pdf", "rb");
	size_t bytes;
	char buffer[65536];

	info = cupsCopyDestInfo(CUPS_HTTP_DEFAULT, dest);

	num_options = cupsAddOption(CUPS_COPIES, "1", num_options, &options);
	num_options = cupsAddOption(CUPS_MEDIA, CUPS_MEDIA_LETTER, num_options, &options);
	num_options = cupsAddOption(CUPS_SIDES, CUPS_SIDES_TWO_SIDED_PORTRAIT, num_options, &options);


	if (cupsCreateDestJob(CUPS_HTTP_DEFAULT, dest, info, &job_id, "My Document", num_options, options) == IPP_STATUS_OK)
	  printf("Created job: %d\n", job_id);
	else
	  printf("Unable to create job: %s\n", cupsLastErrorString());

	if (cupsStartDestDocument(CUPS_HTTP_DEFAULT, dest, info, job_id, "filename.pdf", CUPS_FORMAT_PDF, 0, NULL, 1) == HTTP_STATUS_CONTINUE) {
  	
  		while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    		if (cupsWriteRequestData(CUPS_HTTP_DEFAULT, buffer, bytes) != HTTP_STATUS_CONTINUE)
      		break;

  	
  		if (cupsFinishDestDocument(CUPS_HTTP_DEFAULT, dest, info) == IPP_STATUS_OK)
    		puts("Document send succeeded.");
  		else
    		printf("Document send failed: %s\n", cupsLastErrorString());
	}

	fclose(fp);
}


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

     	if (strcmp( ds[k].name, "ECOM-чб-1") == 0){
     		printf("    %s (%s)\n", ds[k].name, model);
     		printf_pdf(&(ds[k]));
     	}
    }
}


void example_copy_dest_info(){
	
	cups_dest_t *dest;
	
	dest = cupsGetNamedDest(CUPS_HTTP_DEFAULT, "ECOM-чб-1", "");

 	cups_dinfo_t *info = cupsCopyDestInfo(CUPS_HTTP_DEFAULT, dest);
	int can_duplex = cupsCheckDestSupported(CUPS_HTTP_DEFAULT, dest, info, CUPS_COPIES, NULL);
	
	printf("%s %s %d", dest->name, dest->instance, can_duplex);
 	
	cupsFreeDestInfo(info);

 	
}



/*
int print_dest_v0(void *user_data, unsigned flags, cups_dest_t *dest){
  if (dest->instance)
    printf("- %s/%s\n", dest->name, dest->instance);
  else
	printf("- %s\n", dest->name);

  return (1);
}

void example_enum_dests_v0(){
	cupsEnumDests(CUPS_DEST_FLAGS_NONE, 1000, NULL, 0, 0, print_dest_v0, NULL);	
}*/

int main (int argc, char **argv) {
  // example_enum_dests_v0();
  // example_enum_dests_v1();
  example_copy_dest_info();

  return (0);
}


