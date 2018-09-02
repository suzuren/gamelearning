
#ifndef __COMM_MACRO_H_
#define __COMM_MACRO_H_

//@brief  条件检测
#define CHECK_RET(EXPR, RET)\
do{\
  if (!(EXPR)){return (RET);}\
}while(0);

//@brief  条件检测
#define CHECK_VOID(EXPR)\
do{\
  if (!(EXPR)){return;}\
}while(0);

//@brief  条件检测
#define CHECK_LOG_RET(EXPR, RET, FMT, ARG...)\
do{\
  if (!(EXPR)){LOG_ERROR(FMT, ##ARG);return (RET);}\
}while(0);

//@brief  条件检测
#define CHECK_LOG_VOID(EXPR, FMT, ARG...)\
do{\
  if (!(EXPR)){LOG_ERROR(FMT, ##ARG);return;}\
}while(0);




//@brief  ASSERT
#define ASSERT_RET(EXPR, RET)\
do{\
  if (!(EXPR)){assert(false);return (RET);}\
}while(0);

//@brief  ASSERT
#define ASSERT_VOID(EXPR)\
do{\
  if (!(EXPR)){assert(false);return;}\
}while(0);

//@brief  ASSERT
#define ASSERT(EXPR)\
do{\
  if (!(EXPR)){assert(false);}\
}while(0);

//@brief  ASSERT
#define ASSERT_LOG_RET(EXPR, RET, FMT, ARG...)\
do{\
  if (!(EXPR)){LOG_ERROR(FMT, ##ARG);assert(false);return (RET);}\
}while(0);

//@brief  ASSERT
#define ASSERT_LOG_VOID(EXPR, FMT, ARG...)\
do{\
  if (!(EXPR)){LOG_ERROR(FMT, ##ARG);assert(false);return;}\
}while(0);

//@brief  ASSERT
#define ASSERT_LOG(EXPR, FMT, ARG...)\
do{\
  if (!(EXPR)){LOG_ERROR(FMT, ##ARG);assert(false);}\
}while(0);


//@brief	结构体构造函数
#define STRUCT_ZERO(TYPE)\
	TYPE(){memset(this, 0, sizeof(*this));}

#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))



#endif//__SERVERGROUP_COMMON_COMM_MACRO_H__

