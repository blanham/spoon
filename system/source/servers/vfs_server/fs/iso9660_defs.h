#ifndef _ISO9660_H
#define _ISO9660_H



#define VD_TYPE_BOOT			0u
#define VD_TYPE_PRIMARY			1u
#define VD_TYPE_SUPPLEMENTARY		2u
#define VD_TYPE_VOLUME_PARTITION	3u
#define VD_TYPE_TERMINATOR		255u


#define DIR_FLAG	((1<<1))


struct volume_descriptor
{
   unsigned int type : 8;
   char identifier[5];
   unsigned int version : 8;
   unsigned  char unused[2041];
} __attribute__ ((packed));



struct boot_volume_descriptor
{
   unsigned int type : 8;
   char identifier[5];
   unsigned int version : 8;
   unsigned char system_id[32];
   unsigned char boot_id[32];
   unsigned char system_use[1977];
} __attribute__ ((packed));


struct terminator_volume_descriptor
{
   unsigned int type : 8;
   char identifier[5];
   unsigned int version : 8;
   unsigned  char unused[2041];
} __attribute__ ((packed));


struct primary_volume_descriptor
{
   unsigned int type : 8;
   char identifier[5];
   unsigned int version : 8;
   unsigned  char unused1;
   unsigned char system_id[32];
   unsigned char volume_id[32];
   unsigned char unused2[8];
   unsigned long volume_space_size_msb : 32;
   unsigned long volume_space_size_lsb : 32;
   unsigned char unused3[32];
   unsigned int volume_set_size : 32;
   unsigned int volume_sequence_number : 32;
   unsigned int logical_block_size : 32;
   unsigned long path_table_size_msb : 32;
   unsigned long path_table_size_lsb : 32;

   unsigned int loc_typeL_path_table : 32;   
   unsigned int loc_opt_typeL_path_table : 32;   
   unsigned int loc_typeM_path_table : 32;   
   unsigned int loc_opt_typeM_path_table : 32;   

   unsigned char root_directory_record[34];

   unsigned char volume_set_identifier[128];
   unsigned char publisher_identifier[128];
   unsigned char data_preparer_identifier[128];
   unsigned char application_identifier[128];
   unsigned char copyright_file_identifier[37];
   unsigned char abstract_file_identifier[37];
   unsigned char bibliographic_file_identifier[37];
   

   unsigned char creation_datetime[17];
   unsigned char modification_datetime[17];
   unsigned char expiration_datetime[17];
   unsigned char effective_datetime[17];

   unsigned char file_structure_version;
   
   unsigned char reserved2;

   unsigned char application_use[512];
   
   unsigned char reserved3[653];
} __attribute__ ((packed));


struct supplementary_volume_descriptor
{
   unsigned int type : 8;
   char identifier[5];
   unsigned int version : 8;
   unsigned char flags : 8;
   unsigned char system_id[32];
   unsigned char volume_id[32];
   unsigned char unused2[8];
   unsigned long volume_space_size_msb : 32;
   unsigned long volume_space_size_lsb : 32;
   unsigned char unused3[32];
   unsigned int volume_set_size : 32;
   unsigned int volume_sequence_number : 32;
   unsigned int logical_block_size : 32;
   unsigned long path_table_size_msb : 32;
   unsigned long path_table_size_lsb : 32;

   unsigned int loc_typeL_path_table : 32;   
   unsigned int loc_opt_typeL_path_table : 32;   
   unsigned int loc_typeM_path_table : 32;   
   unsigned int loc_opt_typeM_path_table : 32;   

   unsigned char root_directory_record[34];

   unsigned char volume_set_identifier[128];
   unsigned char publisher_identifier[128];
   unsigned char data_preparer_identifier[128];
   unsigned char application_identifier[128];
   unsigned char copyright_file_identifier[37];
   unsigned char abstract_file_identifier[37];
   unsigned char bibliographic_file_identifier[37];
   

   unsigned char creation_datetime[17];
   unsigned char modification_datetime[17];
   unsigned char expiration_datetime[17];
   unsigned char effective_datetime[17];

   unsigned char file_structure_version;
   
   unsigned char reserved2;

   unsigned char application_use[512];
   
   unsigned char reserved3[653];
} __attribute__ ((packed));



struct partition_volume_descriptor
{
   unsigned int type : 8;
   char identifier[5];
   unsigned int version : 8;
   unsigned char unusued1 : 8;
   unsigned char system_id[32];
   unsigned char partition_id[32];
   unsigned long partition_location_msb : 32;
   unsigned long partition_location_lsb : 32;
   unsigned long partition_size_msb : 32;
   unsigned long partition_size_lsb : 32;
   unsigned  char system_use[1960];
} __attribute__ ((packed));



struct directory_record
{
   unsigned char length : 8;
   unsigned int  extended_attribute_len : 8;
   unsigned long location_extent_msb : 32;
   unsigned long location_extent_lsb : 32;
   unsigned long data_length_msb : 32;
   unsigned long data_length_lsb : 32;
   unsigned char recording_datetime[7];
   unsigned char flags : 8;
   unsigned char unit_size : 8;
   unsigned char interleave_size : 8;
   unsigned int  sequence_num : 32;
   unsigned char file_id_len : 8;
   unsigned char file_id[2015]; 
} __attribute__ ((packed));



struct path_table_record
{
   unsigned char length : 8;
   unsigned int  extended_attribute_len : 8;
   unsigned int  location_extent : 32;
   unsigned int  parent_directory_number : 16;
   unsigned char directory_id[2040];
} __attribute__ ((packed));


struct extended_attribute_record
{
   unsigned int owner_id : 32;
   unsigned int group_id : 32;
   unsigned int permissions : 16;


   unsigned char creation_datetime[17];
   unsigned char modification_datetime[17];
   unsigned char expiration_datetime[17];
   unsigned char effective_datetime[17];

   unsigned char record_format : 8;
   unsigned char record_attribute : 8;
   
   unsigned int record_len : 32;
   unsigned char system_id[32];

   unsigned char system_use[64];

   unsigned char extended_attr_record_version : 8;
   unsigned char escape_sequence_len : 8;
   unsigned char reserved[64];
   unsigned int application_use_len : 32;

   unsigned char application_use[1798] ;



   
} __attribute__ ((packed));





#endif





