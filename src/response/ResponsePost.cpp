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

std::string	ResponsePost::parseNameFromMultipart(void)//TODO upgrade this parser
{
	std::string baseName;
	std::string type;

	std::string fileName = this->_contentDisposition["filename"];

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
		baseName = fileName.empty() ? "upload" : fileName;
		type = "dat";
	}

	this->makeUnicIde(baseName, type);

	return baseName;
}


std::string	ResponsePost::getFileName()
{
	std::string fileName;
	std::string baseName;
	std::string type;

	if (this->_contentType == MULTIPART)
	{
		baseName = this->parseNameFromMultipart();
		return baseName;
	}
	std::string rawValues = this->_req.getHeader("Content-Disposition");
	std::vector<std::string> values = tokenizeLine(rawValues);

	std::vector<std::string>::iterator it;
	for (it = values.begin(); it != values.end(); it++)
	{
		if (it->substr(0, 9) == "filename=")
		{
			fileName = it->substr(9);
			trimQuotes(fileName);
			break;
		}
	}

	if (fileName.empty())
	{
		std::string uri = this->_req.getUri();
		size_t lastSlash = uri.find_last_of("/");

		if (lastSlash != std::string::npos && lastSlash + 1 < uri.size())
			fileName = uri.substr(lastSlash + 1);

		if (fileName.empty())
			fileName = "upload";
	}

	size_t	dotPos = fileName.find_last_of(".");
	if (dotPos != std::string::npos && dotPos > 0)
	{
		baseName = fileName.substr(0, dotPos);
		type = fileName.substr(dotPos + 1);
		if (type.empty())
			type = "dat";
	}
	else
	{
		baseName = fileName.empty() ? "upload" : fileName;
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

void	ResponsePost::buildFromMultipart(void)
{
	std::stringstream iss(this->_req.getBody());
	std::string	body = this->_req.getBody();
	std::string	boundaryStart = "--" + this->_boundary;
	std::string	boundaryEnd = boundaryStart + "--";

	size_t pos = body.find(boundaryStart);
	if (pos == std::string::npos)
	{
		LOG_WARNING_LINK("Boundary start not found in multipart body");
		return;
	}

	pos += boundaryStart.length();
	size_t lineEnd = body.find("\r\n", pos);
	if (lineEnd == std::string::npos)
		lineEnd = body.find("\n", pos);
	if (lineEnd != std::string::npos)
		pos = lineEnd + (body[lineEnd] == '\r' ? 2 : 1);

		// ✅ Parse headers until empty line
	while (pos < body.size())
	{
		// Find end of current line
		lineEnd = body.find("\r\n", pos);
		if (lineEnd == std::string::npos)
			lineEnd = body.find("\n", pos);

		if (lineEnd == std::string::npos)
			break;

		std::string line = body.substr(pos, lineEnd - pos);

		// ✅ Empty line marks end of headers
		if (line.empty())
		{
			pos = lineEnd + (body[lineEnd] == '\r' ? 2 : 1);
			break;
		}

		// ✅ Parse Content-Disposition header
		if (line.find("Content-Disposition:") == 0)
		{
			std::string params = line.substr(20); // Skip "Content-Disposition:"
			std::map<std::string, std::string> disposition = parseHeaderParameters(params);

			// ✅ Safe access with find()
			std::map<std::string, std::string>::iterator it = disposition.find("filename");
			if (it != disposition.end())
				this->_fileName = it->second;
			//TODO make unike
		}

		// ✅ Parse Content-Type header
		if (line.find("Content-Type:") == 0)
		{
			std::string params = line.substr(13); // Skip "Content-Type:"
			std::map<std::string, std::string> contentType = parseHeaderParameters(params);

			// ✅ Get first key (the mime type)
			if (!contentType.empty())
				this->_mime = contentType.begin()->first;
		}

		// Move to next line
		pos = lineEnd + (body[lineEnd] == '\r' ? 2 : 1);
	}

	// ✅ Now extract file content (binary safe)
	size_t contentStart = pos;
	size_t nextBoundary = body.find(boundaryStart, pos);

	if (nextBoundary == std::string::npos)
	{
		LOG_WARNING_LINK("Closing boundary not found in multipart body");
		return;
	}

	// ✅ Content ends before the boundary (remove trailing \r\n before boundary)
	size_t contentEnd = nextBoundary;
	if (contentEnd >= 2 && body[contentEnd - 2] == '\r' && body[contentEnd - 1] == '\n')
		contentEnd -= 2;
	else if (contentEnd >= 1 && body[contentEnd - 1] == '\n')
		contentEnd -= 1;

	// ✅ Extract binary-safe file content
	if (contentEnd > contentStart)
		this->_saveFile = body.substr(contentStart, contentEnd - contentStart);
	else
		this->_saveFile.clear();
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

	if (this->_contentType == MULTIPART)
		this->buildFromMultipart();

	std::string savePath;

	Location const* location = this->_cfg.findMatchingLocation(this->_req.getUri());
	if (!location)
	{
		savePath = normalizePath(this->_cfg.getRoot(), "/tmp");
		if (!this->setOrCreatePath(savePath))
			return;
	}
	else
	{
		//TODO check if POST is allowed in this location, if not send 405 method nod allowed. We have refactor to properly store allowed methods first
		savePath = location->getStore();
		if (!this->setOrCreatePath(savePath))
			return;
	}


	savePath += ("/" + this->_fileName);

	if (!isSecurePath(savePath))
	{
		LOG_WARNING_LINK("Response POST build insecure path: [" + savePath + "]");
		responseIsErrorPage(400);
		return;
	}

	std::ofstream file(savePath.c_str(), std::ios::binary);
	if (!file.is_open()) {
		responseIsErrorPage(500);
		return;
	}

	file.write(this->_saveFile.data(), this->_saveFile.size());
	file.close();
	if (file.fail())
	{
		LOG_HIGH_WARNING_LINK("Failed to write file: [" + savePath + "]");
		this->responseIsErrorPage(500);
		return;
	}

	std::string resourceUri = this->_req.getUri();
	if (!resourceUri.empty() && resourceUri[resourceUri.size() - 1] != '/')
		resourceUri += "/";
	resourceUri += this->_fileName;

	this->addHeader("Location", resourceUri);
	this->_bodyIsFile = false;
	this->setStatus(201);
	this->setBody("<html><body><h1>201 Created</h1></body></html>", "text/html");//TODO  this is a placeholder, delete this when implemented a response page for upload

}
