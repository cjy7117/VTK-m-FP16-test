/*#include <vtkm/Types.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DataSetBuilderUniform.h>
#include <vtkm/worklet/Invoker.h>
#include <vtkm/cont/VariantArrayHandle.h>
#include <vtkm/worklet/WorkletMapField.h>
#include <vtkm/worklet/DispatcherMapField.h>
#include <vtkm/cont/DataSetFieldAdd.h>
#include <vtkm/filter/FilterField.h>
#include <vtkm/cont/ArrayHandle.h>
#include <iostream>
#include <vtkm/filter/internal/CreateResult.h> 
#include <string>
#include <vtkm/cont/Field.h>
*/
namespace vtkm
{
namespace worklet
{
class WorkletSquareFP16 : public vtkm::worklet::WorkletMapField
{
  public:
    using ControlSignature = void(FieldIn, FieldOut);
    using ExecutionSignature = _2(_1, WorkIndex);

    VTKM_EXEC
    vtkm::Float16 operator()(vtkm::Float16 x, vtkm::Id&) const
    {
      return x * x;
    }
};
}

namespace filter
{


struct PolicyFP16DataSet : vtkm::filter::PolicyBase<PolicyFP16DataSet>
{
public:
  struct TypeListTagFP16 : vtkm::ListTagBase<::vtkm::UInt8,
                                             ::vtkm::Int32,
                           		     ::vtkm::Int64,
		                             ::vtkm::Float16,
                                             ::vtkm::Float32,
                                             ::vtkm::Float64,
                                             ::vtkm::Vec<::vtkm::Float16, 3>,
                                             ::vtkm::Vec<::vtkm::Float32, 3>,
                                             ::vtkm::Vec<::vtkm::Float64, 3>>
  {
  };

  using FieldTypeList = TypeListTagFP16;
};

class FilterFieldSquareFP16 : public vtkm::filter::FilterField<FilterFieldSquareFP16>
{
  public: 
  template<typename T, typename S, typename Policy>
  cont::DataSet DoExecute(
  const cont::DataSet& inDataSet,
  const cont::ArrayHandle<T, S>& inField,
  const filter::FieldMetadata& fieldMetadata,
  filter::PolicyBase<Policy>)
  {
  //construct our output
  cont::ArrayHandle<vtkm::Float16> outField;

  //construct our invoker to launch worklets
  worklet::Invoker invoker;
  worklet:: WorkletSquareFP16 sf16;
  invoker(sf16, inField, outField); //launch mag worklets

  //for (vtkm::Int64 i = 0; i < 27; i++) {
  //  std::cout << outField.GetPortalConstControl().Get(i) << ", ";
  //} 

  //construct output field information
  if (this->GetOutputFieldName().empty())
  {
    this->SetOutputFieldName( fieldMetadata.GetName() + "_squared");
  }

  return internal::CreateResult(inDataSet,
                                outField,
                                this->GetOutputFieldName(),
                                fieldMetadata.GetAssociation(),
                                fieldMetadata.GetCellSetName());
}


};

template<>
  struct FilterTraits<vtkm::filter::FilterFieldSquareFP16>
  {
    using InputFieldTypeList = vtkm::ListTagBase<vtkm::Float16>;
  };


}


}


