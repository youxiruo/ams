#ifndef AMSENUM_H
#define AMSENUM_H


enum amsServiceGroupFlag
{
	AMS_SERVICE_GROUP_UNINSTALL              = 0,
	AMS_SERVICE_GROUP_INSTALL                = 1,	
};

enum amsServiceState
{
	AMS_SERVICE_INACTIVE = 0,
	AMS_SERVICE_ACTIVE,
};

enum amsServiceFlag
{
	AMS_SERVICE_UNINSTALL                    = 0,
	AMS_SERVICE_INSTALL                      = 1,	
};


enum amsCommonResult
{
	AMS_SUCCESS = AMS_COMMON_REASON_BASE,
	AMS_PARA_ERR,
	AMS_STATE_ERR,
	AMS_ALLOC_MEM_FAILED,
	AMS_ALLOC_LP_FAILED,
	AMS_ALLOC_LT_FAILED,

	AMS_PARA_CFG_INF_CNF_TIMEOUT = 0x001f,
	
	AMS_OTHER_FAILED             = 0x00ff,
};


#endif
