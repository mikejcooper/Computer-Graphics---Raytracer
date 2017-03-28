#pragma OPENCL EXTENSION cl_khr_fp64 : enable




kernel void accelerate_flow(global int* cells,
                            int accel)
{
  printf("%d\n", accel);
}

// -----------------------------------------------------------------------------------------

