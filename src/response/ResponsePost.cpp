#include "ResponsePost.hpp"
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

	// std::cout << "....................................." << std::endl;
	// std::cout << "Content-disposition: " << std::endl;
	// 	std::map<std::string, std::string>::iterator it_d;
	// for (it_d = this->_contentDisposition.begin(); it_d != this->_contentDisposition.end(); it_d++)
	// 	std::cout << "Element: " << it_d->first << "; value = " << it_d->second << std::endl;

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


// std::string	ResponsePost::getFileName()
// {
// 	std::string fileName;
// 	std::string baseName;
// 	std::string type;

// 	if (this->_contentType == MULTIPART)
// 	{
// 		baseName = this->parseNameFromMultipart();
// 		return baseName;
// 	}
// 	std::string rawValues = this->_req.getHeader("Content-Disposition");
// 	std::vector<std::string> values = tokenizeLine(rawValues);

// 	std::vector<std::string>::iterator it;
// 	for (it = values.begin(); it != values.end(); it++)
// 	{
// 		if (it->substr(0, 9) == "filename=")
// 		{
// 			fileName = it->substr(9);
// 			trimQuotes(fileName);
// 			break;
// 		}
// 	}

// 	if (fileName.empty())
// 	{
// 		std::string uri = this->_req.getUri();
// 		size_t lastSlash = uri.find_last_of("/");

// 		if (lastSlash != std::string::npos && lastSlash + 1 < uri.size())
// 			fileName = uri.substr(lastSlash + 1);

// 		if (fileName.empty())
// 			fileName = "upload";
// 	}

// 	size_t	dotPos = fileName.find_last_of(".");
// 	if (dotPos != std::string::npos && dotPos > 0)
// 	{
// 		baseName = fileName.substr(0, dotPos);
// 		type = fileName.substr(dotPos + 1);
// 		if (type.empty())
// 			type = "dat";
// 	}
// 	else
// 	{
// 		baseName = fileName.empty() ? "upload" : fileName;
// 		type = "dat";
// 	}

// 	this->makeUnicIde(baseName, type);

// 	return baseName;
// }

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
	this->printContentTypeElements();

	if (pathIsExecutable(path))
	{
		LOG_HIGH_WARNING_LINK("Path exists but is an executable, not a directory: [" + path + "]");
		this->responseIsErrorPage(500);
		return false;
	}
	if (pathIsRegFile(path))
	{
		LOG_HIGH_WARNING_LINK("Path exists but is a file, not a directory: [" + path + "]");
		this->responseIsErrorPage(500);
		return false;
	}


	if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
	{
		LOG_HIGH_WARNING_LINK("Failed to create direcory: [" + path + "]");
		this->responseIsErrorPage(500);
		return false;
	}

	return true;
}

bool	ResponsePost::buildFromMultipart(void)
{
	std::string bodyFilePath = this->_req.getBodyFilePath();

	if (bodyFilePath.empty())
	{
		LOG_HIGH_WARNING_LINK("No body file path available");
		return false;
	}

	std::ifstream bodyFile(bodyFilePath.c_str(), std::ios::binary);
	if (!bodyFile.is_open())
	{
		LOG_HIGH_WARNING_LINK("Failed to open body file: " + bodyFilePath);
		return false;
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
		return false;
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

	std::ofstream outFile(savePath.c_str(), std::ios::binary);
	if (!outFile.is_open())
	{
		bodyFile.close();
		return false;
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
		return false;
	}

	std::string resourceUri = this->_req.getUri();
	if (!resourceUri.empty() && resourceUri[resourceUri.size() - 1] != '/')
		resourceUri += "/";
	resourceUri += this->_fileName;

	this->addHeader("Location", resourceUri);
	this->_bodyIsFile = false;
	this->setStatus(201);
	this->setBody("<html><body><h1>201 Created</h1></body></html>", "text/html");//TODO  this is a placeholder, delete this when implemented a response page for upload

	return true;
}

std::string	ResponsePost::saveFilePath(void)
{
	std::string savePath;

	Location const* location = this->_cfg.findMatchingLocation(this->_req.getUri());
	if (!location)
	{
		savePath = normalizePath(this->_cfg.getRoot(), "/tmp");
		if (!this->setOrCreatePath(savePath))
			return "";
	}
	else
	{
		savePath = location->getStore();
		if (savePath.empty())
			savePath = normalizePath(this->_cfg.getRoot(), location->getRoot());
		if (!this->setOrCreatePath(savePath))
			return "";
	}
	if (!this->isAllowedMethod("POST"))
	{
		this->responseIsErrorPage(405);
		LOG_WARNING_LINK("Method [POST] not allowed");
		return "";
	}

	savePath += ("/" + this->_fileName);

	if (!isSecurePath(savePath))
	{
		LOG_WARNING_LINK("Response POST build insecure path: [" + savePath + "]");
		responseIsErrorPage(400);
		return "";
	}

	return savePath;
}

/*============== member function =============*/

void	ResponsePost::buildResponse(void)
{
	if (this->_req.getBodySize() > this->_cfg.getMaxBodySize())
	{
		this->responseIsErrorPage(413);
		return;
	}
	//TODO handle first if POST demands CGI. if yes, launch the binary, if not, store body as a file.

	switch (this->_contentType)
	{
	case TEXT:
		LOG_WARNING_LINK("Content type [text/plain] not supported yet");
		this->responseIsErrorPage(500);
		return;
	case MULTIPART:
		if (!this->buildFromMultipart())
			this->responseIsErrorPage(500);
		return;
	case JSON:
		LOG_WARNING_LINK("Content type [application/json] not supported yet");
		this->responseIsErrorPage(500);
		return;
	case URLENCODED:
		LOG_WARNING_LINK("Content type [application/x-www-form-urlencoded] not supported yet");
		this->responseIsErrorPage(500);
		return;
	default:
		LOG_WARNING_LINK("Content type not supported");
		this->responseIsErrorPage(500);
		return;
	}
}
