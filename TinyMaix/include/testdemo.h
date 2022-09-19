#ifndef _TINYMAIXTEST_H_
#define _TINYMAIXTEST_H_


#define TINYMAIX_DEMOUSED    DEMO_MNIST


#define DEMO_MNIST          (1)
#define DEMO_CIFAR          (2)
#define DEMO_VWW            (3)
#define DEMO_MBNET          (4)

#if TINYMAIX_DEMOUSED == DEMO_MNIST
#  define DEMO_MNIST_USED
#  undef  DEMO_CIFAR_USED
#  undef  DEMO_VWW _USED
#  undef  DEMO_MBNET_USED
#elif TINYMAIX_DEMOUSED == DEMO_CIFAR
#  define DEMO_CIFAR_USED
#  undef  DEMO_MNIST_USED
#  undef  DEMO_VWW _USED
#  undef  DEMO_MBNET_USED
#elif TINYMAIX_DEMOUSED == DEMO_VWW
#  define DEMO_VWW_USED
#  undef  DEMO_MNIST_USED
#  undef  DEMO_CIFAR_USED
#  undef  DEMO_MBNET_USED
#elif TINYMAIX_DEMOUSED == DEMO_MBNET
#  define DEMO_MBNET_USED
#  undef  DEMO_MNIST_USED
#  undef  DEMO_CIFAR_USED
#  undef  DEMO_VWW _USED
#endif


int tinymaix_demo(void);

#endif
