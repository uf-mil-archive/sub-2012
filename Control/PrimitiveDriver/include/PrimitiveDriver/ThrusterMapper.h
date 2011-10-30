#ifndef THRUSTERMAPPER_H
#define THRUSTERMAPPER_H

#include <Eigen/Dense>

namespace subjugator {
	class ThrusterMapper {
		public:
			struct Entry {
				Eigen::Vector3d lineofaction;
				Eigen::Vector3d position;
				double fsat, rsat;

				Entry() { }
				Entry(const Eigen::Vector3d &lineofaction, const Eigen::Vector3d position, double fsat, double rsat)
				: lineofaction(lineofaction), position(position), fsat(fsat), rsat(rsat) { }
			};
			typedef Eigen::Matrix<double, 6, 1> Vector6D;

			ThrusterMapper(const Eigen::Vector3d &centerofmass, int entries);

			void setEntry(int num, const Entry &entry);
			void clearEntry(int num);

			Eigen::VectorXd mapWrenchToEfforts(const Vector6D& wrench) const;

		private:
			typedef Eigen::Matrix<double, 6, Eigen::Dynamic> MapMatrix;

			Eigen::Vector3d centerofmass;
			int entries;

			MapMatrix mapmatrix;
			Eigen::VectorXd fsat;
			Eigen::VectorXd rsat;

			mutable Eigen::JacobiSVD<MapMatrix> svd;
			mutable bool svdstale;

			void updateSvd() const;
			Eigen::VectorXd forcesToEfforts(const Eigen::VectorXd &forces) const;
	};
}

#endif /* THRUSTERMAPPER_H_ */
