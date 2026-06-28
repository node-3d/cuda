struct posval { uint32_t pos; float val; };

void thrust_inclusiveScan_( uint32_t *ptr1, uint32_t *ptr2, uint32_t *ptr3 );
void thrust_floatSort_int_(float *ptr1, float *ptr2, uint32_t *ptr3);
int32_t thrust_remove_int_(int32_t *ptr1, int32_t *ptr2, int32_t value);

float thrust_reduce_floatSum_(float *ptr1, float *ptr2);
posval thrust_reduce_floatMax_(float *ptr1, float *ptr2);
posval thrust_reduce_floatMin_(float *ptr1, float *ptr2);
//void thrust_inclusiveScan_(uint32_t *ptr1, uint32_t *ptr2, uint32_t *ptr3);
