#include "DiagnoseWind.h"
#include "../Util.h"
#include "../File/File.h"
#include <cmath>
CalibratorDiagnoseWind::CalibratorDiagnoseWind(const Variable& iVariable, const Options& iOptions) :
      Calibrator(iVariable, iOptions),
      mX(""),
      mY(""),
      mSpeed(""),
      mDirection(""),
      mCompute("") {
   iOptions.getValue("x", mX);
   iOptions.getValue("y", mY);
   iOptions.getValue("speed", mSpeed);
   iOptions.getValue("direction", mDirection);
   iOptions.getRequiredValue("compute", mCompute);
   if((mCompute == "x" || mCompute == "y") && (mSpeed == "" || mDirection == "")) {
      Util::error("Both speed and direction variables must be specified");
   }
   if((mCompute == "speed" || mCompute == "direction") && (mX == "" || mY == "")) {
      Util::error("Both x and y variables must be specified");
   }
}
bool CalibratorDiagnoseWind::calibrateCore(File& iFile, const ParameterFile* iParameterFile) const {
   int nY = iFile.getNumY();
   int nX = iFile.getNumX();
   int nEns = iFile.getNumEns();
   int nTime = iFile.getNumTime();

   // Get all fields
   for(int t = 0; t < nTime; t++) {
      Field& output = *iFile.getField(mVariable, t);
      if(mCompute == "x") {
         const Field& speed = *iFile.getField(mSpeed, t);
         const Field& direction = *iFile.getField(mDirection, t);

         #pragma omp parallel for
         for(int y = 0; y < nY; y++) {
            for(int x = 0; x < nX; x++) {
               for(int e = 0; e < nEns; e++) {
                  if(Util::isValid(speed(y, x, e)) && Util::isValid(direction(y, x, e)))
                     output(y, x, e) = -speed(y, x, e) * sin(direction(y, x, e) / 180.0 * Util::pi);
               }
            }
         }
      }

      else if(mCompute == "y") {
         const Field& speed = *iFile.getField(mSpeed, t);
         const Field& direction = *iFile.getField(mDirection, t);

         #pragma omp parallel for
         for(int y = 0; y < nY; y++) {
            for(int x = 0; x < nX; x++) {
               for(int e = 0; e < nEns; e++) {
                  if(Util::isValid(speed(y, x, e)) && Util::isValid(direction(y, x, e)))
                     output(y, x, e) = -speed(y, x, e) * cos(direction(y, x, e) / 180.0 * Util::pi);
               }
            }
         }
      }

      else if(mCompute == "speed") {
         const Field& X = *iFile.getField(mX, t);
         const Field& Y = *iFile.getField(mY, t);

         #pragma omp parallel for
         for(int y = 0; y < nY; y++) {
            for(int x = 0; x < nX; x++) {
               for(int e = 0; e < nEns; e++) {
                  if(Util::isValid(X(y, x, e)) && Util::isValid(Y(y, x, e)))
                     output(y, x, e) = sqrt(X(y, x, e) * X(y, x, e) + Y(y, x, e) * Y(y, x, e));
               }
            }
         }
      }

      else if(mCompute == "direction") {
         const Field& X = *iFile.getField(mX, t);
         const Field& Y = *iFile.getField(mY, t);

         #pragma omp parallel for
         for(int y = 0; y < nY; y++) {
            for(int x = 0; x < nX; x++) {
               for(int e = 0; e < nEns; e++) {
                  if(Util::isValid(X(y, x, e)) && Util::isValid(Y(y, x, e)))
                     output(y, x, e) = std::atan2(-X(y, x, e), -Y(y, x, e)) * 180 / Util::pi;
               }
            }
         }
      }
   }
   return true;
}
std::string CalibratorDiagnoseWind::description(bool full) {
   std::stringstream ss;
   if(full) {
      ss << Util::formatDescription("-c diagnoseWind","Compute wind speed/direction or x/y components. Specify either x/y or speed/direction options:") << std::endl;
      ss << Util::formatDescription("   x=undef","X-wind variable name") << std::endl;
      ss << Util::formatDescription("   y=undef","Y-wind variable name") << std::endl;
      ss << Util::formatDescription("   speed=undef","Speed variable name") << std::endl;
      ss << Util::formatDescription("   direction=undef","Direction variable name") << std::endl;
      ss << Util::formatDescription("   compute=required","Which variable type should be diagnosed? One of x, y, speed, or direction.") << std::endl;
   }
   else
      ss << Util::formatDescription("-c diagnoseWind","Diagnose wind speed/direction or x/y components") << std::endl;
   return ss.str();
}

