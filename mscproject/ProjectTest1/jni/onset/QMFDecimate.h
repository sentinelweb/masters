/*
 * QMF.h
 *
 *  Created on: 12-Jul-2010
 *      Author: robm
 */

#ifndef QMF_H_
#define QMF_H_
#define 	SQRT2  	1.414213562

class QMFDecimate {


public:
	int size;
	float* block;
	float* decimated;
	float* residual;
	void copyBlock(float* _block);
	void copyBlock(short* _block);
	void filterAndDecimate(bool residual);
	void reconstruct();
	QMFDecimate(int _size);
	virtual ~QMFDecimate();
};

#endif /* QMF_H_ */
