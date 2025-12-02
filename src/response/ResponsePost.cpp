#include "Response_headers/ResponsePost.hpp"
#include "CGIheaders/ACGIexecutor.hpp"
#include "CGIheaders/CGIfactory.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

/*============== constructor and destructor =============*/

ResponsePost::ResponsePost(ServerCfg const& cfg, Request const& req)
: Response(cfg, req), _contentType(UNKNOWNCT), _boundary("")
{
	std::string elements = this->_req.getHeader("Content-Type");
	if (elements.empty())
		return;
	this->_contentTypeElements = parseHeaderParameters(elements);

	this->_contentType = extractContentType();

	this->_boundary = this->_contentTypeElements["boundary"];

	// this->printContentTypeElements();
}

ResponsePost::~ResponsePost() {}

/*============== private member functions =============*/

void	ResponsePost::printContentTypeElements(void)
{

	static const std::string types[] = {"text/plain", "multipart/form-data", "application/json", "application/x-www-form-urlencoded"};

	std::cout << "Content-type elements in POST request" << std::endl;
	std::cout << "Content-type: " << types[this->_contentType] << std::endl;
	std::cout << "Boundary: " << this->_boundary << std::endl;

	std::map<std::string, std::string>::iterator it;
	for (it = this->_contentTypeElements.begin(); it != this->_contentTypeElements.end(); it++)
		std::cout << "Element: " << it->first << "; value = " << it->second << std::endl;
}

ResponsePost::e_contentType ResponsePost::extractContentType()
{
	static const std::string types[] = {"text/plain", "multipart/form-data", "application/json", "application/x-www-form-urlencoded"};
	for (int i = 0; i < ARRAY_SIZE(types); i++)
	{
		if (this->_contentTypeElements.find(types[i]) != this->_contentTypeElements.end())
			return static_cast<e_contentType>(i);
	}
	return UNKNOWNCT;
}
std::string	ResponsePost::normalizeFilename(std::string const& fileName)
{
	std::string baseName;
	std::string type;

	size_t	dotPos = fileName.find_last_of(".");//TODO make this a helper method "findExtension"
	if (dotPos != std::string::npos && dotPos > 0)
	{
		baseName = fileName.substr(0, dotPos);
		type = fileName.substr(dotPos + 1);
		if (type.empty())
			type = "dat";
	}
	else
	{
		baseName = fileName.empty() ? "uploaded_file" : fileName;
		type = "dat";
	}

	this->makeUnicIde(baseName, type);

	return baseName;
}

void	ResponsePost::makeUnicIde(std::string& fileName, std::string const& type)
{
	std::stringstream ss;
	ss << fileName << "_" << time(NULL) << "." << type;
	fileName = ss.str();
}

bool	ResponsePost::setOrCreatePath(std::string const& path)
{
	if (pathIsDirectory(path))
		return true;

	if (pathIsExecutable(path))
	{
		LOG_HIGH_WARNING_LINK("Path exists but is an executable, not a directory: [" + path + "]");
		this->_responseIsErrorPage(500);
		return false;
	}
	if (pathIsRegFile(path))
	{
		LOG_HIGH_WARNING_LINK("Path exists but is a file, not a directory: [" + path + "]");
		this->_responseIsErrorPage(500);
		return false;
	}


	if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
	{
		LOG_HIGH_WARNING_LINK("Failed to create direcory: [" + path + "]");
		this->_responseIsErrorPage(500);
		return false;
	}

	return true;
}

int	ResponsePost::buildFromMultipart(void)
{
	if (this->_boundary.empty())
	{
		LOG_WARNING_LINK("request body has no boundaries");
		return 400;
	}

	std::string bodyFilePath = this->_req.getBodyFilePath();

	if (bodyFilePath.empty())
	{
		LOG_HIGH_WARNING_LINK("No body file path available");
		return 400;
	}

	std::ifstream bodyFile(bodyFilePath.c_str(), std::ios::binary);
	if (!bodyFile.is_open())
	{
		LOG_HIGH_WARNING_LINK("Failed to open body file: " + bodyFilePath);
		return 500;
	}

	// Parse headers (same as above)
	std::string boundaryStart = "--" + this->_boundary;
	std::string boundaryEnd = boundaryStart + "--";
	std::string line;
	bool foundBoundary = false;

	while (std::getline(bodyFile, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		if (line == boundaryStart)
		{
			foundBoundary = true;
			break;
		}
	}

	if (!foundBoundary)
	{
		bodyFile.close();
		LOG_HIGH_WARNING_LINK("Open boundary not found on body file: " + bodyFilePath);
		return 400;
	}

	// Parse headers
	while (std::getline(bodyFile, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		if (line.empty())
			break;

		if (line.find("Content-Disposition:") == 0)
		{
			std::string params = line.substr(20);
			std::map<std::string, std::string> disposition = parseHeaderParameters(params);

			std::map<std::string, std::string>::iterator it = disposition.find("filename");
			if (it != disposition.end())
				this->_fileName = this->normalizeFilename(it->second);
		}

		if (line.find("Content-Type:") == 0)
		{
			std::string params = line.substr(13);
			std::map<std::string, std::string> contentType = parseHeaderParameters(params);

			if (!contentType.empty())
				this->_mime = contentType.begin()->first;
		}
	}

	// Copy directly to final destination
	std::string savePath = this->saveFilePath();
	if (savePath.empty())
	{
		bodyFile.close();
		return -1;//error already set in savefilepath
	}

	std::ofstream outFile(savePath.c_str(), std::ios::binary);
	if (!outFile.is_open())
	{
		bodyFile.close();
		LOG_HIGH_WARNING_LINK("Failed to create output file: " + savePath);
		return 500;
	}

	// Stream copy content until boundary
	char buffer[4096];
	std::string leftover;

	while (bodyFile.read(buffer, sizeof(buffer)) || bodyFile.gcount() > 0)
	{
		std::string chunk(buffer, bodyFile.gcount());
		leftover += chunk;

		// Check for closing boundary
		size_t boundaryPos = leftover.find(boundaryEnd);
		if (boundaryPos != std::string::npos)
		{
			// Write everything before boundary
			if (boundaryPos >= 2 && leftover[boundaryPos - 2] == '\r')
				boundaryPos -= 2;
			else if (boundaryPos >= 1 && leftover[boundaryPos - 1] == '\n')
				boundaryPos -= 1;

			outFile.write(leftover.data(), boundaryPos);
			break;
		}

		// Keep last few bytes in case boundary spans chunks
		if (leftover.size() > boundaryEnd.size())
		{
			size_t writeSize = leftover.size() - boundaryEnd.size();
			outFile.write(leftover.data(), writeSize);
			leftover = leftover.substr(writeSize);
		}
	}

	bodyFile.close();
	outFile.close();

	if (outFile.fail())
	{
		LOG_HIGH_WARNING_LINK("Failed to write file: " + savePath);
		std::remove(savePath.c_str());
		return 500;
	}

	std::string resourceUri = this->_req.getUri();
	if (!resourceUri.empty() && resourceUri[resourceUri.size() - 1] != '/')
		resourceUri += "/";
	resourceUri += this->_fileName;

	this->_addHeader("Location", resourceUri);
	this->_bodyIsFile = false;
	this->_setStatus(201);
	this->_setBody("<html><body><h1>201 Created</h1></body></html>", "text/html");//TODO  this is a placeholder, delete this when implemented a response page for upload

	return 0;
}

std::string	ResponsePost::saveFilePath(void)
{
	std::string savePath;

	Location const* location = this->_cfg.getBestMatchLocation(this->_req.getUri());
	if (!location)
	{
		savePath = _normalizePath(this->_cfg.getRoot(), "/tmp");
		if (!this->setOrCreatePath(savePath))
			return "";
	}
	else
	{
		savePath = location->getStore();
		if (savePath.empty())
		{
			std::string uriPath = this->_req.getUri().substr(location->getLocationPath().size());
			savePath = _normalizePath(location->getRoot(), uriPath);
		}
		if (!this->setOrCreatePath(savePath))
			return "";
	}

	savePath += ("/" + this->_fileName);

	if (!_isSecurePath(savePath))
	{
		LOG_WARNING_LINK("Response POST build insecure path: [" + savePath + "]");
		_responseIsErrorPage(400);
		return "";
	}

	return savePath;
}

/*============== member function =============*/

void	ResponsePost::buildResponse(void)
{
	//TODO handle first if POST demands CGI. if yes, launch the binary, if not, store body as a file.

	if (CGIfactory::isValidCGI(this->_req, this->_cfg))
	{
		ACGIexecutor *CGI = CGIfactory::create(this->_req, this->_cfg);
		if (!CGI)
		{
			LOG_HIGH_WARNING_LINK("CGI allocation failed");
			this->_responseIsErrorPage(500);
		}
		CGI->execute();
		return;
	}

	int errorCode;

	switch (this->_contentType)
	{
	case TEXT:
			LOG_WARNING_LINK("Content type [text/plain] not supported yet");
			this->_responseIsErrorPage(415);
		return;
	case MULTIPART:
		errorCode = this->buildFromMultipart();
		if (errorCode > 0)
			this->_responseIsErrorPage(errorCode);
		return;
	case JSON:
		LOG_WARNING_LINK("Content type [application/json] not supported yet");
		this->_responseIsErrorPage(415);
		return;
	case URLENCODED:
		LOG_WARNING_LINK("Content type [application/x-www-form-urlencoded] not supported yet");
		this->_responseIsErrorPage(415);
		return;
	default:
		LOG_WARNING_LINK("Content type not supported");
		this->_responseIsErrorPage(415);
		return;
	}
}
