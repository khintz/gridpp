#include "Deaccumulate.h"
#include "../Util.h"
#include "../File/File.h"
CalibratorDeaccumulate::CalibratorDeaccumulate(const Variable& iVariable, const Options& iOptions) :
      Calibrator(iVariable, iOptions) {
   iOptions.check();
}
bool CalibratorDeaccumulate::calibrateCore(File& iFile, const ParameterFile* iParameterFile) const {
   int nY = iFile.getNumY();
   int nX = iFile.getNumX();
   int nEns = iFile.getNumEns();
   int nTime = iFile.getNumTime();

   // Get all fields
   std::vector<FieldPtr> fields(nTime);
   std::vector<FieldPtr> fieldsAcc(nTime);
   for(int t = 0; t < nTime; t++) {
      fieldsAcc[t] = iFile.getField(mVariable, t);
      fields[t] = iFile.getEmptyField();
   }

   for(int t = 0; t < nTime; t++) {
      #pragma omp parallel for
      for(int y = 0; y < nY; y++) {
         for(int x = 0; x < nX; x++) {
            for(int e = 0; e < nEns; e++) {
               if(t == 0) {
                  (*fields[t])(y, x, e) = Util::MV;
               }
               else {
                  float previous = (*fieldsAcc[t - 1])(y, x, e);
                  float current  = (*fieldsAcc[t])(y, x, e);
                  if(Util::isValid(current) && Util::isValid(previous)) {
                     (*fields[t])(y, x, e) = current - previous;
                  }
                  else {
                     (*fields[t])(y, x, e) = Util::MV;
                  }
               }
            }
         }
      }

      iFile.addField(fields[t], mVariable, t);
   }
   return true;
}
std::string CalibratorDeaccumulate::description() {
   std::stringstream ss;
   ss << Util::formatDescription("-c deaccumulate","Deaccumlates a value over time. It is assumed that the raw value for time t is the accumulated precip on the interval [0,t]. After deaccumulation, the value for time t is then the value over the interval [t-1,t]. Thus, the deaccumulated value for the first timestep will be missing.") << std::endl;
   return ss.str();
}