//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#

#ifndef ANNOTATIONS_ANNRECTBOX_H
#define ANNOTATIONS_ANNRECTBOX_H

#include <casa/aips.h>
#include <images/Annotations/AnnRegion.h>

namespace casa {

// <summary>
// This class represents an annotation for rectangular (in position coordinates) region specified
// in an ascii region file as proposed in CAS-2285
// </summary>
// <author>Dave Mehringer</author>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// <prerequisite>

// </prerequisite>

// <etymology>
// Holds the specification of an annotation for a rectangular region as specified in ASCII format.
// </etymology>

// <synopsis>
// This class represents an annotaton for a rectangular region in coordinate space.
// </synopsis>


class AnnRectBox: public AnnRegion {

public:

	AnnRectBox(
		const Quantity& blcx,
		const Quantity& blcy,
		const Quantity& trcx,
		const Quantity& trcy,
		const String& dirRefFrameString,
		const CoordinateSystem& csys,
		const Quantity& beginFreq,
		const Quantity& endFreq,
		const String& freqRefFrameString,
		const String& dopplerString,
		const Quantity& restfreq,
		const Vector<Stokes::StokesTypes> stokes,
		const Bool annotationOnly
	);

	// get the blc and trc direction coords for the box.
	// The output directions will be converted from the input
	// reference frame to the reference frame of the input
	// coordinate system if necessary.
	// blc is the 0th component, trc the 1st in the returned vector.

	Vector<MDirection> getCorners() const;

private:
	Matrix<Quantity> _inputCorners;

	// disallow default constructor
	AnnRectBox();
};

}

#endif
