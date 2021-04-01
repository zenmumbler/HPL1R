/*
 * 2021 by zenmumbler
 * This file is part of Rehatched
 */

#include "angelscript.h"

#include "script/Script.h"
#include "resources/ResourceManager.h"
#include "system/LowLevelSystem.h"
#include "impl/Platform.h"
#include "system/String.h"

#include "script/impl/scriptarray.h"
#include "script/impl/scriptstdstring.h"
#include "script/impl/scripthelper.h"

namespace hpl {

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// cScriptOutput (internal type)
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	class cScriptOutput
	{
	public:
		cScriptOutput() : msMessage("") {}
		~cScriptOutput() = default;

		void AddMessage(const asSMessageInfo *msg);
		void Display();
		void Clear();
		
		const tString& GetMessage(){ return msMessage; }

	private:
		tString msMessage;
	};

	void cScriptOutput::AddMessage(const asSMessageInfo *msg) {
		char sMess[1024];

		tString type = "ERR ";
		if( msg->type == asMSGTYPE_WARNING )
			type = "WARN";
		else if( msg->type == asMSGTYPE_INFORMATION )
			type = "INFO";

		snprintf(sMess, 1024, "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type.c_str(), msg->message);

		msMessage += sMess;
	}
	
	void cScriptOutput::Display() {
		if (msMessage.size() > 500)
		{
			while(msMessage.size() > 500)
			{
				tString sSub = msMessage.substr(0, 500);
				msMessage = msMessage.substr(500);
				Log(sSub.c_str());
			}
			Log(msMessage.c_str());
		}
		else
		{
			Log(msMessage.c_str());
		}
	}
	
	void cScriptOutput::Clear() {
		msMessage = "";
	}
	

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// cScriptModule
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cScriptModule::cScriptModule(const tString& asName,asIScriptEngine *apScriptEngine,
							cScriptOutput *apScriptOutput)
		: iResourceBase(asName, 0)
		, mpEngine{apScriptEngine}
		, mpOutput{apScriptOutput}
	{
		mpContext = mpEngine->CreateContext();
	}

	cScriptModule::~cScriptModule()
	{
		mpModule->Discard();
		mpContext->Release();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cScriptModule::CreateFromFile(const tString& asFileName)
	{
		int lLength;
		char *pCharBuffer = LoadCharBuffer(asFileName,lLength);
		if(pCharBuffer==NULL){
			Error("Couldn't load script '%s'!\n",asFileName.c_str());
			return false;
		}

		tString moduleName = "Module_" + cString::GetFileName(GetName());
		mpModule = mpEngine->GetModule(moduleName.c_str(), asGM_ALWAYS_CREATE);
		if(mpModule->AddScriptSection("main", pCharBuffer, lLength)<0)
		{
			Error("Couldn't add script '%s'!\n",asFileName.c_str());
			hplDeleteArray(pCharBuffer);
			return false;
		}

		int lBuildOutput = mpModule->Build();
//		if (apCompileMessages) {
//			*apCompileMessages = mpScriptOutput->GetMessage();
//		}

		bool success = lBuildOutput >= 0;
		if (! success)
		{
			Error("Couldn't build script '%s'!\n",asFileName.c_str());
			Log("------- SCRIPT OUTPUT BEGIN --------------------------\n");
			mpOutput->Display();
			Log("------- SCRIPT OUTPUT END ----------------------------\n");
		}
		mpOutput->Clear();

		hplDeleteArray(pCharBuffer);
		return success;
	}

	bool cScriptModule::Run(const tString& asFuncLine)
	{
		ExecuteString(mpEngine, asFuncLine.c_str(), mpModule);
		return true;
	}

	char* cScriptModule::LoadCharBuffer(const tString& asFileName, int& alLength)
	{
		FILE *pFile = fopen(asFileName.c_str(), "rb");
		if (pFile == NULL) {
			return NULL;
		}

		int lLength = (int)Platform::FileLength(pFile);
		alLength = lLength;

		char *pBuffer = hplNewArray(char,lLength);
		fread(pBuffer, lLength, 1, pFile);

		fclose(pFile);

		return pBuffer;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// cScript
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cScript::cScript(cResources *resources)
		: iUpdateable("HPL_Scripting"), mpResources(resources)
	{
		mpEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		if (mpEngine == NULL) {
			Error("Failed to start angel script!\n");
		}

		mpOutput = hplNew( cScriptOutput, () );
		mpEngine->SetMessageCallback(asMETHOD(cScriptOutput,AddMessage), mpOutput, asCALL_THISCALL);

		RegisterScriptArray(mpEngine, true);
		RegisterStdString(mpEngine);
		RegisterStdStringUtils(mpEngine);
	}
	
	cScript::~cScript() {
		mpEngine->Release();
		hplDelete(mpOutput);
	}

	cScriptModule *cScript::CreateScript(const tString &asName) {
		return hplNew( cScriptModule, (asName, mpEngine, mpOutput) );
	}
	
	bool cScript::AddScriptFunc(const tString &asFuncDecl, void *pFunc) {
		if (mpEngine->RegisterGlobalFunction(asFuncDecl.c_str(), asFUNCTION(pFunc), asCALL_CDECL) < 0)
		{
			Error("Couldn't add func '%s'\n", asFuncDecl.c_str());
			return false;
		}

		return true;
	}
	
	bool cScript::AddScriptVar(const tString &asVarDecl, void *pVar) {
		if (mpEngine->RegisterGlobalProperty(asVarDecl.c_str(),pVar)<0)
		{
			Error("Couldn't add var '%s'\n", asVarDecl.c_str());
			return false;
		}

		return true;
	}
	
	void cScript::Update(float afTimeStep) {
		// nothing yet
	}
}
