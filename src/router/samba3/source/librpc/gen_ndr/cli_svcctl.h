#include "librpc/gen_ndr/ndr_svcctl.h"
#ifndef __CLI_SVCCTL__
#define __CLI_SVCCTL__
NTSTATUS rpccli_svcctl_CloseServiceHandle(struct rpc_pipe_client *cli,
					  TALLOC_CTX *mem_ctx,
					  struct policy_handle *handle /* [in,out] [ref] */,
					  WERROR *werror);
NTSTATUS rpccli_svcctl_ControlService(struct rpc_pipe_client *cli,
				      TALLOC_CTX *mem_ctx,
				      struct policy_handle *handle /* [in] [ref] */,
				      uint32_t control /* [in]  */,
				      struct SERVICE_STATUS *service_status /* [out] [ref] */,
				      WERROR *werror);
NTSTATUS rpccli_svcctl_DeleteService(struct rpc_pipe_client *cli,
				     TALLOC_CTX *mem_ctx,
				     struct policy_handle *handle /* [in] [ref] */,
				     WERROR *werror);
NTSTATUS rpccli_svcctl_LockServiceDatabase(struct rpc_pipe_client *cli,
					   TALLOC_CTX *mem_ctx,
					   struct policy_handle *handle /* [in] [ref] */,
					   struct policy_handle *lock /* [out] [ref] */,
					   WERROR *werror);
NTSTATUS rpccli_svcctl_QueryServiceObjectSecurity(struct rpc_pipe_client *cli,
						  TALLOC_CTX *mem_ctx,
						  struct policy_handle *handle /* [in] [ref] */,
						  uint32_t security_flags /* [in]  */,
						  uint8_t *buffer /* [out] [ref,size_is(buffer_size)] */,
						  uint32_t buffer_size /* [in] [range(0,0x40000)] */,
						  uint32_t *needed /* [out] [ref,range(0,0x40000)] */,
						  WERROR *werror);
NTSTATUS rpccli_svcctl_SetServiceObjectSecurity(struct rpc_pipe_client *cli,
						TALLOC_CTX *mem_ctx,
						struct policy_handle *handle /* [in] [ref] */,
						uint32_t security_flags /* [in]  */,
						uint8_t *buffer /* [in] [ref,size_is(buffer_size)] */,
						uint32_t buffer_size /* [in]  */,
						WERROR *werror);
NTSTATUS rpccli_svcctl_QueryServiceStatus(struct rpc_pipe_client *cli,
					  TALLOC_CTX *mem_ctx,
					  struct policy_handle *handle /* [in] [ref] */,
					  struct SERVICE_STATUS *service_status /* [out] [ref] */,
					  WERROR *werror);
NTSTATUS rpccli_svcctl_SetServiceStatus(struct rpc_pipe_client *cli,
					TALLOC_CTX *mem_ctx,
					WERROR *werror);
NTSTATUS rpccli_svcctl_UnlockServiceDatabase(struct rpc_pipe_client *cli,
					     TALLOC_CTX *mem_ctx,
					     struct policy_handle *lock /* [in,out] [ref] */,
					     WERROR *werror);
NTSTATUS rpccli_svcctl_NotifyBootConfigStatus(struct rpc_pipe_client *cli,
					      TALLOC_CTX *mem_ctx,
					      WERROR *werror);
NTSTATUS rpccli_svcctl_SCSetServiceBitsW(struct rpc_pipe_client *cli,
					 TALLOC_CTX *mem_ctx,
					 struct policy_handle *handle /* [in] [ref] */,
					 uint32_t bits /* [in]  */,
					 uint32_t bitson /* [in]  */,
					 uint32_t immediate /* [in]  */,
					 WERROR *werror);
NTSTATUS rpccli_svcctl_ChangeServiceConfigW(struct rpc_pipe_client *cli,
					    TALLOC_CTX *mem_ctx,
					    struct policy_handle *handle /* [in] [ref] */,
					    uint32_t type /* [in]  */,
					    uint32_t start /* [in]  */,
					    uint32_t error /* [in]  */,
					    const char *binary_path /* [in] [unique,charset(UTF16)] */,
					    const char *load_order_group /* [in] [unique,charset(UTF16)] */,
					    uint32_t *tag_id /* [out] [ref] */,
					    const char *dependencies /* [in] [unique,charset(UTF16)] */,
					    const char *service_start_name /* [in] [unique,charset(UTF16)] */,
					    const char *password /* [in] [unique,charset(UTF16)] */,
					    const char *display_name /* [in] [unique,charset(UTF16)] */,
					    WERROR *werror);
NTSTATUS rpccli_svcctl_CreateServiceW(struct rpc_pipe_client *cli,
				      TALLOC_CTX *mem_ctx,
				      struct policy_handle *scmanager_handle /* [in] [ref] */,
				      const char *ServiceName /* [in] [charset(UTF16)] */,
				      const char *DisplayName /* [in] [unique,charset(UTF16)] */,
				      uint32_t desired_access /* [in]  */,
				      uint32_t type /* [in]  */,
				      uint32_t start_type /* [in]  */,
				      uint32_t error_control /* [in]  */,
				      const char *binary_path /* [in] [charset(UTF16)] */,
				      const char *LoadOrderGroupKey /* [in] [unique,charset(UTF16)] */,
				      uint32_t *TagId /* [in,out] [unique] */,
				      uint8_t *dependencies /* [in] [unique,size_is(dependencies_size)] */,
				      uint32_t dependencies_size /* [in]  */,
				      const char *service_start_name /* [in] [unique,charset(UTF16)] */,
				      uint8_t *password /* [in] [unique,size_is(password_size)] */,
				      uint32_t password_size /* [in]  */,
				      struct policy_handle *handle /* [out] [ref] */,
				      WERROR *werror);
NTSTATUS rpccli_svcctl_EnumDependentServicesW(struct rpc_pipe_client *cli,
					      TALLOC_CTX *mem_ctx,
					      struct policy_handle *service /* [in] [ref] */,
					      uint32_t state /* [in]  */,
					      uint8_t *service_status /* [out] [ref,size_is(buf_size)] */,
					      uint32_t buf_size /* [in] [range(0,0x40000)] */,
					      uint32_t *bytes_needed /* [out] [ref,range(0,0x40000)] */,
					      uint32_t *services_returned /* [out] [ref,range(0,0x40000)] */,
					      WERROR *werror);
NTSTATUS rpccli_svcctl_EnumServicesStatusW(struct rpc_pipe_client *cli,
					   TALLOC_CTX *mem_ctx,
					   struct policy_handle *handle /* [in] [ref] */,
					   uint32_t type /* [in]  */,
					   uint32_t state /* [in]  */,
					   uint32_t buf_size /* [in]  */,
					   uint8_t *service /* [out] [size_is(buf_size)] */,
					   uint32_t *bytes_needed /* [out] [ref] */,
					   uint32_t *services_returned /* [out] [ref] */,
					   uint32_t *resume_handle /* [in,out] [unique] */,
					   WERROR *werror);
NTSTATUS rpccli_svcctl_OpenSCManagerW(struct rpc_pipe_client *cli,
				      TALLOC_CTX *mem_ctx,
				      const char *MachineName /* [in] [unique,charset(UTF16)] */,
				      const char *DatabaseName /* [in] [unique,charset(UTF16)] */,
				      uint32_t access_mask /* [in]  */,
				      struct policy_handle *handle /* [out] [ref] */,
				      WERROR *werror);
NTSTATUS rpccli_svcctl_OpenServiceW(struct rpc_pipe_client *cli,
				    TALLOC_CTX *mem_ctx,
				    struct policy_handle *scmanager_handle /* [in] [ref] */,
				    const char *ServiceName /* [in] [charset(UTF16)] */,
				    uint32_t access_mask /* [in]  */,
				    struct policy_handle *handle /* [out] [ref] */,
				    WERROR *werror);
NTSTATUS rpccli_svcctl_QueryServiceConfigW(struct rpc_pipe_client *cli,
					   TALLOC_CTX *mem_ctx,
					   struct policy_handle *handle /* [in] [ref] */,
					   struct QUERY_SERVICE_CONFIG *query /* [out] [ref] */,
					   uint32_t buf_size /* [in] [range(0,8192)] */,
					   uint32_t *bytes_needed /* [out] [ref,range(0,8192)] */,
					   WERROR *werror);
NTSTATUS rpccli_svcctl_QueryServiceLockStatusW(struct rpc_pipe_client *cli,
					       TALLOC_CTX *mem_ctx,
					       struct policy_handle *handle /* [in] [ref] */,
					       uint32_t buf_size /* [in]  */,
					       struct SERVICE_LOCK_STATUS *lock_status /* [out] [ref] */,
					       uint32_t *required_buf_size /* [out] [ref] */,
					       WERROR *werror);
NTSTATUS rpccli_svcctl_StartServiceW(struct rpc_pipe_client *cli,
				     TALLOC_CTX *mem_ctx,
				     struct policy_handle *handle /* [in] [ref] */,
				     uint32_t NumArgs /* [in]  */,
				     const char *Arguments /* [in] [unique,charset(UTF16)] */,
				     WERROR *werror);
NTSTATUS rpccli_svcctl_GetServiceDisplayNameW(struct rpc_pipe_client *cli,
					      TALLOC_CTX *mem_ctx,
					      struct policy_handle *handle /* [in] [ref] */,
					      const char *service_name /* [in] [unique,charset(UTF16)] */,
					      const char **display_name /* [out] [ref,charset(UTF16)] */,
					      uint32_t *display_name_length /* [in,out] [unique] */,
					      WERROR *werror);
NTSTATUS rpccli_svcctl_GetServiceKeyNameW(struct rpc_pipe_client *cli,
					  TALLOC_CTX *mem_ctx,
					  struct policy_handle *handle /* [in] [ref] */,
					  const char *service_name /* [in] [unique,charset(UTF16)] */,
					  const char **key_name /* [out] [ref,charset(UTF16)] */,
					  uint32_t *display_name_length /* [in,out] [unique] */,
					  WERROR *werror);
NTSTATUS rpccli_svcctl_SCSetServiceBitsA(struct rpc_pipe_client *cli,
					 TALLOC_CTX *mem_ctx,
					 struct policy_handle *handle /* [in] [ref] */,
					 uint32_t bits /* [in]  */,
					 uint32_t bitson /* [in]  */,
					 uint32_t immediate /* [in]  */,
					 WERROR *werror);
NTSTATUS rpccli_svcctl_ChangeServiceConfigA(struct rpc_pipe_client *cli,
					    TALLOC_CTX *mem_ctx,
					    struct policy_handle *handle /* [in] [ref] */,
					    uint32_t type /* [in]  */,
					    uint32_t start /* [in]  */,
					    uint32_t error /* [in]  */,
					    const char *binary_path /* [in] [unique,charset(UTF16)] */,
					    const char *load_order_group /* [in] [unique,charset(UTF16)] */,
					    uint32_t *tag_id /* [out] [ref] */,
					    const char *dependencies /* [in] [unique,charset(UTF16)] */,
					    const char *service_start_name /* [in] [unique,charset(UTF16)] */,
					    const char *password /* [in] [unique,charset(UTF16)] */,
					    const char *display_name /* [in] [unique,charset(UTF16)] */,
					    WERROR *werror);
NTSTATUS rpccli_svcctl_CreateServiceA(struct rpc_pipe_client *cli,
				      TALLOC_CTX *mem_ctx,
				      struct policy_handle *handle /* [in] [ref] */,
				      const char *ServiceName /* [in] [unique,charset(UTF16)] */,
				      const char *DisplayName /* [in] [unique,charset(UTF16)] */,
				      uint32_t desired_access /* [in]  */,
				      uint32_t type /* [in]  */,
				      uint32_t start_type /* [in]  */,
				      uint32_t error_control /* [in]  */,
				      const char *binary_path /* [in] [unique,charset(UTF16)] */,
				      const char *LoadOrderGroupKey /* [in] [unique,charset(UTF16)] */,
				      uint32_t *TagId /* [out] [unique] */,
				      const char *dependencies /* [in] [unique,charset(UTF16)] */,
				      const char *service_start_name /* [in] [unique,charset(UTF16)] */,
				      const char *password /* [in] [unique,charset(UTF16)] */,
				      WERROR *werror);
NTSTATUS rpccli_svcctl_EnumDependentServicesA(struct rpc_pipe_client *cli,
					      TALLOC_CTX *mem_ctx,
					      struct policy_handle *service /* [in] [ref] */,
					      uint32_t state /* [in]  */,
					      struct ENUM_SERVICE_STATUS *service_status /* [out] [unique] */,
					      uint32_t buf_size /* [in]  */,
					      uint32_t *bytes_needed /* [out] [ref] */,
					      uint32_t *services_returned /* [out] [ref] */,
					      WERROR *werror);
NTSTATUS rpccli_svcctl_EnumServicesStatusA(struct rpc_pipe_client *cli,
					   TALLOC_CTX *mem_ctx,
					   struct policy_handle *handle /* [in] [ref] */,
					   uint32_t type /* [in]  */,
					   uint32_t state /* [in]  */,
					   uint32_t buf_size /* [in]  */,
					   uint8_t *service /* [out] [size_is(buf_size)] */,
					   uint32_t *bytes_needed /* [out] [ref] */,
					   uint32_t *services_returned /* [out] [ref] */,
					   uint32_t *resume_handle /* [in,out] [unique] */,
					   WERROR *werror);
NTSTATUS rpccli_svcctl_OpenSCManagerA(struct rpc_pipe_client *cli,
				      TALLOC_CTX *mem_ctx,
				      const char *MachineName /* [in] [unique,charset(UTF16)] */,
				      const char *DatabaseName /* [in] [unique,charset(UTF16)] */,
				      uint32_t access_mask /* [in]  */,
				      struct policy_handle *handle /* [out] [ref] */,
				      WERROR *werror);
NTSTATUS rpccli_svcctl_OpenServiceA(struct rpc_pipe_client *cli,
				    TALLOC_CTX *mem_ctx,
				    struct policy_handle *scmanager_handle /* [in] [ref] */,
				    const char *ServiceName /* [in] [unique,charset(UTF16)] */,
				    uint32_t access_mask /* [in]  */,
				    WERROR *werror);
NTSTATUS rpccli_svcctl_QueryServiceConfigA(struct rpc_pipe_client *cli,
					   TALLOC_CTX *mem_ctx,
					   struct policy_handle *handle /* [in] [ref] */,
					   uint8_t *query /* [out]  */,
					   uint32_t buf_size /* [in]  */,
					   uint32_t *bytes_needed /* [out] [ref] */,
					   WERROR *werror);
NTSTATUS rpccli_svcctl_QueryServiceLockStatusA(struct rpc_pipe_client *cli,
					       TALLOC_CTX *mem_ctx,
					       struct policy_handle *handle /* [in] [ref] */,
					       uint32_t buf_size /* [in]  */,
					       struct SERVICE_LOCK_STATUS *lock_status /* [out] [ref] */,
					       uint32_t *required_buf_size /* [out] [ref] */,
					       WERROR *werror);
NTSTATUS rpccli_svcctl_StartServiceA(struct rpc_pipe_client *cli,
				     TALLOC_CTX *mem_ctx,
				     struct policy_handle *handle /* [in] [ref] */,
				     uint32_t NumArgs /* [in]  */,
				     const char *Arguments /* [in] [unique,charset(UTF16)] */,
				     WERROR *werror);
NTSTATUS rpccli_svcctl_GetServiceDisplayNameA(struct rpc_pipe_client *cli,
					      TALLOC_CTX *mem_ctx,
					      struct policy_handle *handle /* [in] [ref] */,
					      const char *service_name /* [in] [unique,charset(UTF16)] */,
					      const char **display_name /* [out] [ref,charset(UTF16)] */,
					      uint32_t *display_name_length /* [in,out] [unique] */,
					      WERROR *werror);
NTSTATUS rpccli_svcctl_GetServiceKeyNameA(struct rpc_pipe_client *cli,
					  TALLOC_CTX *mem_ctx,
					  struct policy_handle *handle /* [in] [ref] */,
					  const char *service_name /* [in] [unique,charset(UTF16)] */,
					  const char **key_name /* [out] [ref,charset(UTF16)] */,
					  uint32_t *display_name_length /* [in,out] [unique] */,
					  WERROR *werror);
NTSTATUS rpccli_svcctl_GetCurrentGroupeStateW(struct rpc_pipe_client *cli,
					      TALLOC_CTX *mem_ctx,
					      WERROR *werror);
NTSTATUS rpccli_svcctl_EnumServiceGroupW(struct rpc_pipe_client *cli,
					 TALLOC_CTX *mem_ctx,
					 WERROR *werror);
NTSTATUS rpccli_svcctl_ChangeServiceConfig2A(struct rpc_pipe_client *cli,
					     TALLOC_CTX *mem_ctx,
					     struct policy_handle *handle /* [in] [ref] */,
					     uint32_t info_level /* [in]  */,
					     uint8_t *info /* [in] [unique] */,
					     WERROR *werror);
NTSTATUS rpccli_svcctl_ChangeServiceConfig2W(struct rpc_pipe_client *cli,
					     TALLOC_CTX *mem_ctx,
					     struct policy_handle *handle /* [in] [ref] */,
					     uint32_t info_level /* [in]  */,
					     uint8_t *info /* [in] [unique] */,
					     WERROR *werror);
NTSTATUS rpccli_svcctl_QueryServiceConfig2A(struct rpc_pipe_client *cli,
					    TALLOC_CTX *mem_ctx,
					    struct policy_handle *handle /* [in] [ref] */,
					    uint32_t info_level /* [in]  */,
					    uint8_t *buffer /* [out]  */,
					    uint32_t buf_size /* [in]  */,
					    uint32_t *bytes_needed /* [out] [ref] */,
					    WERROR *werror);
NTSTATUS rpccli_svcctl_QueryServiceConfig2W(struct rpc_pipe_client *cli,
					    TALLOC_CTX *mem_ctx,
					    struct policy_handle *handle /* [in] [ref] */,
					    uint32_t info_level /* [in]  */,
					    uint8_t *buffer /* [out]  */,
					    uint32_t buf_size /* [in]  */,
					    uint32_t *bytes_needed /* [out] [ref] */,
					    WERROR *werror);
NTSTATUS rpccli_svcctl_QueryServiceStatusEx(struct rpc_pipe_client *cli,
					    TALLOC_CTX *mem_ctx,
					    struct policy_handle *handle /* [in] [ref] */,
					    uint32_t info_level /* [in]  */,
					    uint8_t *buffer /* [out]  */,
					    uint32_t buf_size /* [in]  */,
					    uint32_t *bytes_needed /* [out] [ref] */,
					    WERROR *werror);
NTSTATUS rpccli_EnumServicesStatusExA(struct rpc_pipe_client *cli,
				      TALLOC_CTX *mem_ctx,
				      struct policy_handle *scmanager /* [in] [ref] */,
				      uint32_t info_level /* [in]  */,
				      uint32_t type /* [in]  */,
				      uint32_t state /* [in]  */,
				      uint8_t *services /* [out]  */,
				      uint32_t buf_size /* [in]  */,
				      uint32_t *bytes_needed /* [out] [ref] */,
				      uint32_t *service_returned /* [out] [ref] */,
				      uint32_t *resume_handle /* [in,out] [unique] */,
				      const char **group_name /* [out] [ref,charset(UTF16)] */,
				      WERROR *werror);
NTSTATUS rpccli_EnumServicesStatusExW(struct rpc_pipe_client *cli,
				      TALLOC_CTX *mem_ctx,
				      struct policy_handle *scmanager /* [in] [ref] */,
				      uint32_t info_level /* [in]  */,
				      uint32_t type /* [in]  */,
				      uint32_t state /* [in]  */,
				      uint8_t *services /* [out]  */,
				      uint32_t buf_size /* [in]  */,
				      uint32_t *bytes_needed /* [out] [ref] */,
				      uint32_t *service_returned /* [out] [ref] */,
				      uint32_t *resume_handle /* [in,out] [unique] */,
				      const char **group_name /* [out] [ref,charset(UTF16)] */,
				      WERROR *werror);
NTSTATUS rpccli_svcctl_SCSendTSMessage(struct rpc_pipe_client *cli,
				       TALLOC_CTX *mem_ctx,
				       WERROR *werror);
#endif /* __CLI_SVCCTL__ */
