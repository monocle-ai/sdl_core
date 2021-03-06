/*
 Copyright (c) 2015, Ford Motor Company
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following
 disclaimer in the documentation and/or other materials provided with the
 distribution.

 Neither the name of the Ford Motor Company nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SRC_COMPONENTS_POLICY_POLICY_EXTERNAL_INCLUDE_POLICY_SQL_PT_REPRESENTATION_H_
#define SRC_COMPONENTS_POLICY_POLICY_EXTERNAL_INCLUDE_POLICY_SQL_PT_REPRESENTATION_H_

#include <string>
#include <vector>
#include "policy/policy_table/types.h"
#include "policy/pt_representation.h"
#include "rpc_base/rpc_base.h"

namespace policy_table = rpc::policy_table_interface_base;

namespace utils {
namespace dbms {
class SQLDatabase;
class SQLQuery;
}  // namespace dbms
}  // namespace utils

namespace policy {

class SQLPTRepresentation : public virtual PTRepresentation {
 public:
  SQLPTRepresentation();
  explicit SQLPTRepresentation(bool in_memory);
  ~SQLPTRepresentation();
  virtual void CheckPermissions(const PTString& app_id,
                                const PTString& hmi_level,
                                const PTString& rpc,
                                CheckPermissionResult& result);

  virtual bool IsPTPreloaded();
  virtual int IgnitionCyclesBeforeExchange();
  virtual int KilometersBeforeExchange(int current);
  virtual bool SetCountersPassedForSuccessfulUpdate(int kilometers,
                                                    int days_after_epoch);
  virtual int DaysBeforeExchange(uint16_t current);
  virtual void IncrementIgnitionCycles();
  virtual void ResetIgnitionCycles();
  virtual int TimeoutResponse();
  virtual bool SecondsBetweenRetries(std::vector<int>* seconds);
  virtual bool RefreshDB();
  virtual std::vector<UserFriendlyMessage> GetUserFriendlyMsg(
      const std::vector<std::string>& msg_codes, const std::string& language);

  virtual EndpointUrls GetUpdateUrls(int service_type);

  virtual int GetNotificationsNumber(const std::string& priority);
  virtual bool GetPriority(const std::string& policy_app_id,
                           std::string* priority);
  InitResult Init(const PolicySettings* settings);
  bool Close();
  bool Clear();
  bool Drop();
  virtual void WriteDb();
  virtual std::shared_ptr<policy_table::Table> GenerateSnapshot() const;
  virtual bool Save(const policy_table::Table& table);
  bool GetInitialAppData(const std::string& app_id,
                         StringArray* nicknames = NULL,
                         StringArray* app_hmi_types = NULL);
  bool GetFunctionalGroupings(policy_table::FunctionalGroupings& groups);

#ifdef BUILD_TESTS
  uint32_t open_counter() {
    return open_counter_;
  }
#endif  // BUILD_TESTS
 protected:
  enum TypeAccess { kAllowed, kManual };
  bool GatherModuleType(const std::string& app_id,
                        policy_table::ModuleTypes* module_types) const;
  bool GatherRemoteControlDenied(const std::string& app_id, bool* denied) const;
  bool GatherAccessModule(TypeAccess access,
                          policy_table::AccessModules* modules) const;
  bool GatherRemoteRpc(int module_id, policy_table::RemoteRpcs* rpcs) const;
  bool SaveModuleType(const std::string& app_id,
                      const policy_table::ModuleTypes& types);
  bool SaveRemoteControlDenied(const std::string& app_id, bool deny);

  bool SaveAccessModule(TypeAccess access,
                        const policy_table::AccessModules& modules);
  bool SaveRemoteRpc(int module_id, const policy_table::RemoteRpcs& rpcs);

  virtual void GatherModuleMeta(policy_table::ModuleMeta* meta) const;
  virtual void GatherModuleConfig(policy_table::ModuleConfig* config) const;
  virtual bool GatherUsageAndErrorCounts(
      policy_table::UsageAndErrorCounts* counts) const;
  virtual void GatherDeviceData(policy_table::DeviceData* data) const;
  virtual bool GatherFunctionalGroupings(
      policy_table::FunctionalGroupings* groups) const;
  virtual bool GatherConsumerFriendlyMessages(
      policy_table::ConsumerFriendlyMessages* messages) const;
  virtual bool GatherApplicationPoliciesSection(
      policy_table::ApplicationPoliciesSection* policies) const;
  virtual bool GatherVehicleDataItems(
      policy_table::VehicleDataItems* vehicle_data_items) const;
  virtual bool GatherVehicleData(policy_table::VehicleData* vehicle_data) const;

  bool GatherAppGroup(const std::string& app_id,
                      policy_table::Strings* app_groups) const;
  bool GatherAppType(const std::string& app_id,
                     policy_table::AppHMITypes* app_types) const;
  bool GatherRequestType(const std::string& app_id,
                         policy_table::RequestTypes* request_types) const;
  bool GatherRequestSubType(
      const std::string& app_id,
      policy_table::RequestSubTypes* request_subtypes) const;
  bool GatherAppServiceParameters(
      const std::string& app_id,
      policy_table::AppServiceParameters* app_service_parameters) const;
  bool GatherNickName(const std::string& app_id,
                      policy_table::Strings* nicknames) const;

  virtual bool SaveApplicationCustomData(const std::string& app_id,
                                         bool is_revoked,
                                         bool is_default,
                                         bool is_predata);

  virtual bool SaveModuleMeta(const policy_table::ModuleMeta& meta);
  virtual bool SaveModuleConfig(const policy_table::ModuleConfig& config);
  virtual bool SaveUsageAndErrorCounts(
      const policy_table::UsageAndErrorCounts& counts);
  virtual bool SaveDeviceData(const policy_table::DeviceData& devices);
  virtual bool SaveFunctionalGroupings(
      const policy_table::FunctionalGroupings& groups);
  virtual bool SaveConsumerFriendlyMessages(
      const policy_table::ConsumerFriendlyMessages& messages);
  virtual bool SaveApplicationPoliciesSection(
      const policy_table::ApplicationPoliciesSection& policies);
  virtual bool SaveSpecificAppPolicy(
      const policy_table::ApplicationPolicies::value_type& app);
  virtual bool SaveDevicePolicy(const policy_table::DevicePolicy& device);
  virtual bool SaveVehicleDataItems(
      const policy_table::VehicleDataItems& vehicle_data_items);
  virtual bool SaveVehicleData(const policy_table::VehicleData& vehicle_data);

  virtual bool SaveMessageString(const std::string& type,
                                 const std::string& lang,
                                 const policy_table::MessageString& strings);

  bool SaveAppGroup(const std::string& app_id,
                    const policy_table::Strings& app_groups);
  bool SaveNickname(const std::string& app_id,
                    const policy_table::Strings& nicknames);
  bool SaveAppType(const std::string& app_id,
                   const policy_table::AppHMITypes& types);
  bool SaveRequestType(const std::string& app_id,
                       const policy_table::RequestTypes& types);
  bool SaveRequestSubType(
      const std::string& app_id,
      const policy_table::RequestSubTypes& request_subtypes);
  bool SaveAppServiceParameters(
      const std::string& app_id,
      const policy_table::AppServiceParameters& app_service_parameters);

 public:
  bool UpdateRequired() const;
  void SaveUpdateRequired(bool value);

  bool IsApplicationRepresented(const std::string& app_id) const;
  bool CopyApplication(const std::string& source,
                       const std::string& destination);

  bool IsApplicationRevoked(const std::string& app_id) const;
  virtual bool IsDefaultPolicy(const std::string& app_id) const;
  virtual bool IsPredataPolicy(const std::string& app_id) const;
  virtual bool SetDefaultPolicy(const std::string& app_id);
  virtual void SetPreloaded(bool value);

  virtual bool SetVINValue(const std::string& value);

  virtual utils::dbms::SQLDatabase* db() const;
  virtual bool SetIsDefault(const std::string& app_id, bool is_default) const;

  void RemoveDB() const OVERRIDE;
  virtual bool IsDBVersionActual() const OVERRIDE;
  virtual bool UpdateDBVersion() const OVERRIDE;

  policy_table::VehicleDataItems GetVehicleDataItem(
      const std::string& name, const std::string& key) const;
  bool InsertVehicleDataItem(
      const policy_table::VehicleDataItem& vehicle_data_item);
  bool VehicleDataItemExists(
      const policy_table::VehicleDataItem& vehicle_data_item) const;

  /**
   * @brief Retrieves vehicle data items of type Struct (which contains params)
   * from the database including these parameters.
   * @return Array of parameterized VehicleDataItems (including params)
   */
  policy_table::VehicleDataItems SelectCompositeVehicleDataItems() const;

  /**
   * @brief Retrieves vehicle data items of non Struct type from the database.
   * @return Array of primitive VehicleDataItems
   */
  policy_table::VehicleDataItems SelectPrimitiveVehicleDataItems() const;

 private:
  utils::dbms::SQLDatabase* db_;

#ifdef BUILD_TESTS
  uint32_t open_counter_;
#endif  // BUILD_TESTS
  enum ExternalConsentEntitiesType {
    kExternalConsentEntitiesTypeOn,
    kExternalConsentEntitiesTypeOff
  };
  /**
   * @brief Calculates DB version from current schema
   * @return version
   */
  const int32_t GetDBVersion() const;
  bool SaveRpcs(int64_t group_id, const policy_table::Rpc& rpcs);
  bool SaveServiceEndpoints(const policy_table::ServiceEndpoints& endpoints);
  bool SaveServiceEndpointProperties(
      const policy_table::ServiceEndpointProperties& endpoint_properties);
  bool SaveSecondsBetweenRetries(
      const policy_table::SecondsBetweenRetries& seconds);
  bool SaveNumberOfNotificationsPerMinute(
      const policy_table::NumberOfNotificationsPerMinute& notifications);
  bool SaveMessageType(const std::string& type);
  bool SaveLanguage(const std::string& code);
  policy_table::VehicleDataItem PopulateVDIFromQuery(
      const utils::dbms::SQLQuery& query) const;
  bool DeleteVehicleDataItems() const;

  bool is_in_memory;
  bool SaveExternalConsentEntities(
      const int64_t group_id,
      const policy_table::DisallowedByExternalConsentEntities& entities,
      ExternalConsentEntitiesType type) const;
};
}  //  namespace policy

#endif  // SRC_COMPONENTS_POLICY_POLICY_EXTERNAL_INCLUDE_POLICY_SQL_PT_REPRESENTATION_H_
