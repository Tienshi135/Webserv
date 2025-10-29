#include "ResponsePost.hpp"
#include "Request.hpp"
#include "Configuration.hpp"

/*============== constructor and destructor =============*/

ResponsePost::ResponsePost(ServerCfg const& cfg, Request const& req)
: Response(cfg, req)
{
	std::string elements = this->_req.getHeader("Content-Type");
	if (elements.empty())
		return;
	this->_contentTypeElements = parseHeaderParameters(elements);
	std::map<std::string, std::string>::iterator it = this->_contentTypeElements.begin();//TODO implement stronger assignment method
	this->_contentType = it->first;

	std::string boundary = this->_contentTypeElements["boundary"];

	size_t beginContent = this->_req.getBody().find(boundary);
	if (beginContent == std::string::npos)
	{
		LOG_WARNING_LINK("boundary begin not found");
		return;
	}

	size_t endContent = this->_req.getBody().find(boundary, beginContent - 1);
	if (endContent == std::string::npos)
	{
		LOG_WARNING_LINK("boundary end not found");
		return;
	}
//check npos

	std::string Content = this->_req.getBody().substr(beginContent, endContent);
	this->_contentDisposition = parseHeaderParameters(Content);

	this->printContentTypeElements();
}

ResponsePost::~ResponsePost() {}

/*============== private member functions =============*/

void	ResponsePost::printContentTypeElements(void)
{
	std::cout << "Content-type elements in POST request" << std::endl;
	std::cout << "Content-type: " << this->_contentType << std::endl;
	std::map<std::string, std::string>::iterator it;
	for (it = this->_contentTypeElements.begin(); it != this->_contentTypeElements.end(); it++)
		std::cout << "Element: " << it->first << "; value = " << it->second << std::endl;
	std::cout << "....................................." << std::endl;
	std::cout << "Content-disposition: " << std::endl;
		std::map<std::string, std::string>::iterator it_d;
	for (it_d = this->_contentDisposition.begin(); it_d != this->_contentDisposition.end(); it_d++)
		std::cout << "Element: " << it_d->first << "; value = " << it_d->second << std::endl;

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

	if (!this->_contentType.empty() && this->_contentType == "multipart/form-data")
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


/*============== member function =============*/

void	ResponsePost::buildResponse(void)
{
	if (this->_req.getBodySize() > this->_cfg.getMaxBodySize())
	{
		this->responseIsErrorPage(413);
		return;
	}
	//TODO handle first if POST demands CGI. if yes, launch the binary, if not, store body as a file.

	std::string savePath;
	std::string	fileName;

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


	fileName = this->getFileName();
	savePath += ("/" + fileName);

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

	file.write(this->_req.getBody().data(), this->_req.getBodySize());
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
	resourceUri += fileName;

	this->addHeader("Location", resourceUri);
	this->_bodyIsFile = false;
	this->setStatus(201);
	this->setBody("<html><body><h1>201 Created</h1></body></html>", "text/html");//TODO  this is a placeholder, delete this when implemented a response page for upload

}
