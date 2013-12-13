#ifndef _SPOON_BASE_FLATTENABLE_H
#define _SPOON_BASE_FLATTENABLE_H



/** \addtogroup base 
 *
 *
 */
class Flattenable
{
	public:
		Flattenable();
		virtual ~Flattenable();

		virtual int Flatten(void *address, int numBytes) const;
		virtual int Unflatten(void *address);
		virtual int FlattenedSize(void) const;

		int	getFlatCode() const;

	private:
		int	m_flattenCode;
};





#endif

