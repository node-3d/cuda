#include <thrust/device_ptr.h>
#include <thrust/for_each.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/remove.h>
#include <thrust/sort.h>
#include <thrust/scan.h>
#include <thrust/extrema.h>

#include "thrust_func.cuh"


void thrust_inclusiveScan_( uint32_t *ptr1, uint32_t *ptr2, uint32_t *ptr3 ){
	thrust::inclusive_scan(
		thrust::device_ptr<uint32_t>( ptr1 ),
		thrust::device_ptr<uint32_t>( ptr2 ),
		thrust::device_ptr<uint32_t>( ptr3 )
	);
};

void thrust_floatSort_int_(float *ptr1, float *ptr2, uint32_t *ptr3){
	thrust::sort_by_key(
		thrust::device_ptr<float>(ptr1),
		thrust::device_ptr<float>(ptr2),
		thrust::device_ptr<uint32_t >(ptr3)
	);
};

int32_t thrust_remove_int_(int32_t *ptr1, int32_t *ptr2, int32_t value){
	thrust::device_ptr<int32_t> new_end = thrust::remove(
		thrust::device_ptr<int32_t>(ptr1),
		thrust::device_ptr<int32_t>(ptr2),
		value
	);

	return new_end.get() - ptr1;
};


float thrust_reduce_floatSum_(float *ptr1, float *ptr2){
	return thrust::reduce(
		thrust::device_ptr<float>(ptr1),
		thrust::device_ptr<float>(ptr2),
		0.0,
		thrust::plus<float>()
	);
};

posval thrust_reduce_floatMax_(float *ptr1, float *ptr2){
	thrust::device_ptr<float> dev_ptr1 = thrust::device_ptr<float>(ptr1);
	thrust::device_ptr<float> dev_ptr2 = thrust::device_ptr<float>(ptr2);
	thrust::device_ptr<float> max_ptr  = thrust::max_element( dev_ptr1, dev_ptr2 );

	posval rv;

	rv.pos = &max_ptr[0] - &dev_ptr1[0];
	// �� �������, ��� �� ��������� ������ �� ������ ����������?
	rv.val = max_ptr[0];

	return rv;
};

posval thrust_reduce_floatMin_(float *ptr1, float *ptr2){
	thrust::device_ptr<float> dev_ptr1 = thrust::device_ptr<float>(ptr1);
	thrust::device_ptr<float> dev_ptr2 = thrust::device_ptr<float>(ptr2);
	thrust::device_ptr<float> min_ptr = thrust::min_element(dev_ptr1, dev_ptr2);

	posval rv;

	rv.pos = &min_ptr[0] - &dev_ptr1[0];
	// �� �������, ��� �� ��������� ������ �� ������ ����������?
	rv.val = min_ptr[0];

	return rv;
};
