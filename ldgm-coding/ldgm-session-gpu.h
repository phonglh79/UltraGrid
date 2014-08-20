/*
 * =====================================================================================
 *
 *       Filename:  ldgm-session-gpu.h
 *
 *    Description:  GPU implementation fo LDGM coding
 *
 *        Version:  1.0
 *        Created:  04/12/2012 12:54:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *        Authors:  Milan Kabat (kabat@ics.muni.cz), Vojtech David (374572@mail.muni.cz)
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  LDGM_SESSION_GPU_INC
#define  LDGM_SESSION_GPU_INC

#include <string.h>
#include <stdio.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include "ldgm-session.h"

#include <map>
#include <queue>

// CUDA check error
#define cuda_check_error(msg) \
    { \
        cudaError_t err = cudaGetLastError(); \
        if( cudaSuccess != err) { \
            fprintf(stderr, "[LDGM GPU] [Error] %s (line %i): %s: %s.\n", \
                __FILE__, __LINE__, msg, cudaGetErrorString( err) ); \
            exit(-1); \
        } \
    } \

/*
 * =====================================================================================
 *        Class:  LDGM_session_gpu
 *  Description:  
 * =====================================================================================
 */
class LDGM_session_gpu : public LDGM_session
{
    public:

    // int * error_vec;
    // int * sync_vec;

    // int * ERROR_VEC;
    // int * SYNC_VEC;



	/* ====================  LIFECYCLE     ======================================= */
	LDGM_session_gpu () {
		printf("GPU LDGM in progress .... \n");

		error_vec=NULL;
		sync_vec=NULL;

		ERROR_VEC=NULL;
		SYNC_VEC=NULL;

		PCM=NULL;

		OUTBUF_SIZE=0;
		OUTBUF=NULL;


	}                             /* constructor      */
	LDGM_session_gpu ( const LDGM_session_gpu &other );   /* copy constructor */
	~LDGM_session_gpu () {
		printf("LDGM TIME GPU: %f ms\n",this->elapsed_sum2/(double)this->no_frames2 );
		cudaError_t error;
                while (!freeBuffers.empty()) {
                        char *buf = freeBuffers.front();
                        freeBuffers.pop();
                        error = cudaFreeHost(buf);
                        if(error != cudaSuccess)printf("memoryPool: %s\n", cudaGetErrorString(error));
                }

		error = cudaFreeHost(error_vec);
		cuda_check_error("error_vec");
		error = cudaFreeHost(sync_vec);
		cuda_check_error("sync_vec");


  //   	cudaFree(OUTBUF);
    	cudaFree(PCM);

    	cudaFree(ERROR_VEC);
    	cudaFree(SYNC_VEC);

	}                           /* destructor       */

	/* ====================  ACCESSORS     ======================================= */

	/* ====================  MUTATORS      ======================================= */

	/* ====================  OPERATORS     ======================================= */

	void 
	    encode ( char* data_ptr, char* parity_ptr );
	
	void 
	    encode_naive ( char* /* data_ptr */, char* /* parity_ptr */ ) {}

	void
	    decode();

	void
	    free_out_buf (char *buf);

	 void *
		alloc_buf(int size);

	char * decode_frame ( char* received_data, int buf_size, int* frame_size, std::map<int, int> valid_data );
	void set_data_fname(char fname[32]) { strncpy(data_fname, fname, 32); }

    protected:
	/* ====================  DATA MEMBERS  ======================================= */

    private:
	/* ====================  DATA MEMBERS  ======================================= */
	char data_fname[32];

        std::queue<char *> freeBuffers;
        std::map<char *, size_t> bufferSizes;

	int OUTBUF_SIZE;
	int * OUTBUF;

	int * error_vec;
	int * sync_vec;

    int * SYNC_VEC;
	int * ERROR_VEC;

	int * PCM;



}; /* -----  end of class LDGM_session_gpu  ----- */

#endif   /* ----- #ifndef LDGM_SESSION_GPU_INC  ----- */
