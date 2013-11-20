section .data
	F3	   dd 0.33333334
	G3	   dd 0.16666667
	one    dd 1.0
	tval   dd 0.6
	retval dd 32.0

	perm   db 151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,\
			  140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,\
			  247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,\
			   57,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,\
			   74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,\
			   60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,\
			   65, 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,\
			  200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,\
			   52,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,\
			  207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,\
			  119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,\
			  129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,\
			  218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,\
			   81, 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,\
			  184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,\
			  222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180,\
			                                                                  \
			  151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,\
	          140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,\
			  247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,\
			   57,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,\
			   74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,\
			   60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,\
			   65, 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,\
			  200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,\
			   52,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,\
			  207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,\
			  119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,\
			  129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,\
			  218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,\
			   81, 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,\
			  184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,\
			  222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180

	grad3  db 1,1,0,  -1,1,0,  1,-1,0,  -1,-1,0,\
			  1,0,1,  -1,0,1,  1,0,-1,  -1,0,-1,\
			  0,1,1,  0,-1,1,  0,1,-1,  0,-1,-1

section .text
	global asm_raw_noise_3d

	asm_raw_noise_3d:
;regs free to use: rax,rcx,rdx,r8,r9,r10,r11
;xmm6-15 = callee save on w64
	movups [rsp-16],xmm6
	movups [rsp-32],xmm7
	movups [rsp-48],xmm8
	movups [rsp-64],xmm9
	movups [rsp-80],xmm10
	movups [rsp-96],xmm11
	movups [rsp-112],xmm12
	movups [rsp-128],xmm13
;0=x 1=y 2=z

	unpcklps xmm0,xmm1
	addss    xmm1,xmm0
	addss    xmm1,xmm2
	movlhps  xmm0,xmm2
	mulss    xmm1,[F3]
	shufps   xmm1,xmm1,0
	addps    xmm1,xmm0
	roundps  xmm1,xmm1,1
;0=xyz 1=ijk

	pshufd   xmm2,xmm1,1
	movhlps  xmm3,xmm1
	addss    xmm2,xmm1
	vbroadcastss xmm6,[G3]
	addss    xmm2,xmm3
	mulss    xmm2,xmm6
	shufps   xmm2,xmm2,0
	vsubps   xmm2,xmm1,xmm2
	subps    xmm0,xmm2
;0=x0y0z0 1=ijk 6=G3

; asm_raw_noise_3d:
; unpcklps xmm0,xmm1
; movlhps xmm0,xmm2

	vbroadcastss xmm7,[one]
	pshufd xmm2,xmm0,000001b ;yxx
	pshufd xmm3,xmm0,100110b ;zyz
	movaps xmm4,xmm2
	cmpps  xmm2,xmm3,5 ;y>=z  x>=y  x>=z
	cmpps  xmm4,xmm3,1 ;z> y  y> x  z> x
	movss  xmm3,xmm4
	shufps xmm2,xmm2,010010b
	insertps xmm4,xmm2,10001000b
	movlhps xmm2,xmm3
	andps   xmm2,xmm4
	andps   xmm2,xmm7

movaps xmm3,xmm1 ;temp

	pshufd   xmm1,xmm0,01b
	pshufd   xmm4,xmm0,10b
	minss    xmm1,xmm4
	cmpss    xmm1,xmm0,2
	andps    xmm1,xmm7      ;i2
	pshufd   xmm4,xmm0,01b
	pshufd   xmm8,xmm0,10b
	cmpss    xmm8,xmm4,2
	andps    xmm8,xmm7
	cmpss    xmm4,xmm0,6
	andps    xmm4,xmm7
	orps     xmm4,xmm8      ;j2
	addss    xmm7,xmm7
	subss    xmm7,xmm1
	subss    xmm7,xmm4      ;k2
	unpcklps xmm1,xmm4
	movlhps  xmm1,xmm7
; pshufd xmm0,xmm2,2
; ret
;0=x0y0z0 1=i2j2k2 2=i1j1k1 3=ijk 6=G3

	vbroadcastss xmm8,[one]
	vsubps   xmm4,xmm0,xmm2
	addps    xmm4,xmm6
	vsubps   xmm5,xmm0,xmm1
	addps    xmm6,xmm6
	addps    xmm5,xmm6
	vsubps   xmm7,xmm0,xmm8
	addss    xmm6,[G3] ;vbroadcastss xmm6,[F3]
	shufps   xmm6,xmm6,0
	addps    xmm7,xmm6
;0=x0y0z0 1=i2j2k2 2=i1j1k1 3=ijk 4=x1y1z1 5=x2y2z2 7=x3y3z3

	cvtps2dq xmm3,xmm3
	cvtps2dq xmm2,xmm2
	cvtps2dq xmm1,xmm1

	pextrb   r9  ,xmm3,8 ;kk
	pextrb   r10 ,xmm2,8 ;k1
	pextrb   r11 ,xmm1,8 ;k2
	movzx    rax ,byte [perm+r9]
	movzx    rcx,byte [perm+r9+r10]
	movzx    rdx,byte [perm+r9+r11]
	movzx    r8 ,byte [perm+r9+1]

	pextrb   r9d ,xmm3,4 ;jj
	pextrb   r10d,xmm2,4
	pextrb   r11d,xmm1,4
	mov      al ,byte [perm+r9+rax]
	add      r10,rcx
	mov      cl ,byte [perm+r9+r10]
	add      r11,rdx
	mov      dl ,byte [perm+r9+r11]
	mov      r8b,byte [perm+r9+r8+1]

	pextrb   r9 ,xmm3,0 ;ii
	pextrb   r10,xmm2,0
	pextrb   r11,xmm1,0
	mov      al ,byte [perm+r9+rax]
	add      r10,rcx
	mov      cl ,byte [perm+r9+r10]
	add      r11,rdx
	mov      dl ,byte [perm+r9+r11]
	mov      r8b,byte [perm+r9+r8+1]

	mov      r9,12
	div      r9b
	shr      ax,8                   ;gi0
	pmovsxbd xmm1,[grad3+rax+rax*2] ;grad3[gi0]
	mov      rax,rcx
	div      r9b
	shr      ax,8
	pmovsxbd xmm2,[grad3+rax+rax*2]
	mov      rax,rdx
	div      r9b
	shr      ax,8
	pmovsxbd xmm3,[grad3+rax+rax*2]
	mov      rax,r8
	div      r9b
	shr      ax,8
	pmovsxbd xmm6,[grad3+rax+rax*2]

	cvtdq2ps xmm1,xmm1
	cvtdq2ps xmm2,xmm2
	cvtdq2ps xmm3,xmm3
	cvtdq2ps xmm6,xmm6
;0=x0y0z0 1=grad3[gi0] 2=grad3[gi1] 3=grad3[gi2] 4=x1y1z1 5=x2y2z2 6=grad3[gi3] 7=x3y3z3

	vmulps   xmm8,xmm0,xmm0
	vmulps   xmm9,xmm4,xmm4
	vmulps   xmm10,xmm5,xmm5
	vmulps   xmm11,xmm7,xmm7

	vunpcklps xmm12,xmm8,xmm10  ;0819
	vunpcklps xmm13,xmm9,xmm11  ;4C5D
	unpckhps  xmm8,xmm10        ;2A3B
	unpckhps  xmm9,xmm11        ;6E7F

	vunpcklps xmm10,xmm12,xmm13 ;048C
	unpckhps  xmm12,xmm13       ;159D
	unpcklps  xmm8,xmm9         ;26AE

	vbroadcastss xmm9,[tval]
	subps     xmm9,xmm10
	subps     xmm9,xmm12
	subps     xmm9,xmm8 ;t0t1t2t3

	pxor      xmm8,xmm8
	cmpps     xmm8,xmm9,1
	andps     xmm8,xmm9
	mulps     xmm8,xmm8
	mulps     xmm8,xmm8

	dpps      xmm1,xmm0,01110001b
	dpps      xmm2,xmm4,01110001b
	dpps      xmm3,xmm5,01110001b
	dpps      xmm6,xmm7,01110001b

	unpcklps  xmm1,xmm2
	unpcklps  xmm3,xmm6
	movlhps   xmm1,xmm3
	mulps     xmm8,xmm1
;8=n0n1n2n3

	haddps    xmm8,xmm8
	haddps    xmm8,xmm8
	vmulss    xmm0,xmm8,[retval]

	movups xmm6,[rsp-16]
	movups xmm7,[rsp-32]
	movups xmm8,[rsp-48]
	movups xmm9,[rsp-64]
	movups xmm10,[rsp-80]
	movups xmm11,[rsp-96]
	movups xmm12,[rsp-112]
	movups xmm13,[rsp-128]
    ret