#include <irrlicht.h>

#include <cstddef>
#include <sstream>
#include <string>

namespace irr
{

namespace io
{

class InMemoryFile: public IReadFile
{
public:
	InMemoryFile(const io::path& filename, const std::string& s);

	std::size_t read(void* buffer, std::size_t sizeToRead) override;
	bool seek(long finalPos, bool relativeMovement=false) override;
	long getSize() const override;
	long getPos() const override;
	const io::path& getFileName() const override;

private:
	io::path m_filename;
	mutable std::stringstream m_sstream;
};

} // namespace io

} // namespace irr

