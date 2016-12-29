#include "MyParser.h"
#include <iostream>

//============ Modifier =================
Modifier::Modifier() {
	this->isPublic = false;
	this->isPrivate = false;
	this->isProtected = false;
	this->isStatic = false;
	this->isFinal = false;
	this->isAbstract = false;
	this->isSynchronized = false;
	this->isVolatile = false;
	this->isTransient = false;
	this->isNative = false;
	this->returnType = new char[255];
	this->returnType[0] = '\0';
}

Modifier::~Modifier() {}

void Modifier::reset() {
	this->isPublic = false;
	this->isPrivate = false;
	this->isProtected = false;
	this->isStatic = false;
	this->isFinal = false;
	this->isAbstract = false;
	this->isSynchronized = false;
	this->isVolatile = false;
	this->isTransient = false;
	this->isNative = false;
	this->returnType = new char[255];
	this->returnType[0] = '\0';
}

void Modifier::setIsPublic(bool isPublic) {
	this->isPublic = isPublic;
}

void Modifier::setIsPrivate(bool isPrivate) {
	this->isPrivate = isPrivate;
}

void Modifier::setIsProtected(bool isProtected) {
	this->isProtected = isProtected;
}

void Modifier::setIsStatic(bool isStatic){
	this->isStatic = isStatic;
}

void Modifier::setIsFinal(bool isFinal){
	this->isFinal = isFinal;
}

void Modifier::setIsAbstract(bool isAbstract){
	this->isAbstract = isAbstract;
}

void Modifier::setIsNative(bool isNative){
	this->isNative = isNative;
}

void Modifier::setIsSynchronized(bool isSynchronized) {
	this->isSynchronized = isSynchronized;
}

void Modifier::setIsTransient(bool isTransient){
	this->isTransient = isTransient;
}

void Modifier::setIsVolatile(bool isVolatile) {
	this->isVolatile = isVolatile;
}

void Modifier::setReturnType(char* returnType) {
	strcat(this->returnType, returnType);
}

bool Modifier::getIsPublic() {
	return this->isPublic;
}

bool Modifier::getIsPrivate() {
	return this->isPrivate;
}

bool Modifier::getIsProtected() {
	return this->isProtected;
}

bool Modifier::getIsStatic() {
	return this->isStatic;
}

bool Modifier::getIsFinal(){
	return this->isFinal;
}

bool Modifier::getIsAbstract(){
	return this->isAbstract;
}

bool Modifier::getIsNative(){
	return this->isNative;
}

bool Modifier::getIsSynchronized() {
	return this->isSynchronized;
}

bool Modifier::getIsTransient() {
	return this->isTransient;
}

bool Modifier::getIsVolatile() {
	return this->isVolatile;
}

char* Modifier::getReturnType() {
	return this->returnType;
}
//================Helper====================
Helper::Helper() {}
Helper::~Helper() {}

void Helper::brcktsCountInc() {
	this->bracketsCount++;
}

void Helper::brcktsCountDec() {
	this->bracketsCount--;
}

void Helper::setBracketsCount(int brcktsCount) {
	this->bracketsCount = brcktsCount;
}

int Helper::getBracketsCount() {
	return this->bracketsCount;
}


MyParser::MyParser(void)
{
	this->st = new SymbolTable();
	this->errRecovery = new ErrorRecovery();
	this->helper = new Helper();
	this->names = new char*[20];
	this->initNames();
}

MyParser::~MyParser(void) {}

void MyParser::initNames() {
	for (int i = 0; i < (sizeof(this->names) / sizeof(**this->names)); i++)
	{
		this->names[i]	  = new char[255];
		this->names[i][0] = '\0';
	}
}

void MyParser::checkBrcktsEquality(int lineNo, int colNo) {
	if (this->helper->getBracketsCount() > 0) {
		this->errRecovery->errQ->enqueue(lineNo, colNo, "Unexpected end of file", "");
	}
	else if (this->helper->getBracketsCount() < 0)
	{
		this->errRecovery->errQ->enqueue(lineNo, colNo, "Unexpected {", "");
	}
}
//========= Variable =================
void MyParser::insertVar(int lineNo, int colNo, Modifier* m) {
	for (int i = 0; i < (sizeof(this->names) / sizeof(**this->names)); i++)
	{
		if (this->names[i] && this->names[i][0]) {
			Variable * v = st->insertVariableInCurrentScope(this->names[i], m);
			cout << "=================================================\n";
			if (!v) {
				cout << "Error variable " << v->getName() << " already defined!";
				this->errRecovery->errQ->enqueue(lineNo, colNo, "Variable is already declared", v->getName());
				return;
			}
			cout << "Variable " << v->getName() << " has been created\n";
			cout << "with return type " << v->getType() << endl;
			if (v->getIsFinal())
				cout << " and it is Final" << endl;
			cout << "=================================================\n";
		}
		else {
			m->reset();
			return;
		}
	}
}
Variable* MyParser::addVariableToCurrentScope(Variable* v) {
	if(v) {
		this->st->currScope->m->put(v->getName(), v, LOCALVARIABLE);
	}
	return v;
}
//========= Data Member =================
void MyParser::insertMem(int lineNo, int colNo, Modifier* m) {
	for (int i = 0; i < (sizeof(this->names) / sizeof(**this->names)); i++)
	{
		if (this->names[i] && this->names[i][0]) {
			DataMember * d = st->insertDataMemberInCurrentScope(this->names[i], m);
			cout << "==============================================\n";
			if (!d) {

				cout << "Error[" << lineNo << ", " << colNo << "]:  data member " << this->names[i] << " already defined!";
				this->errRecovery->errQ->enqueue(lineNo, colNo, "Data member is already declared", this->names[i]);
				return;
			}

			// Checking if function has different modifiers
			if (d->illegalCombinationOfModifiers()) {
				cout << "==================================================\n";
				cout << "Error[" << lineNo << ", " << colNo << "]: Illegal combination of modifiers\n";
				cout << "==================================================\n";
				return;
			}
			d->printDetails();
		}
		else {
			m->reset();
			return;
		}
	}
}
DataMember* MyParser::addDataMemberToCurrentScope(DataMember* d) {
	if (d) {
		this->st->currScope->m->put(d->getName(), d, DATAMEMBER);
	}
	return d;
}
//========= Types =================
Type * MyParser::createType(char* name, int lineno, int colno, Modifier* m){
	Type* t = (Type*)this->st->currScope->m->get(name);
	cout << "=============== Class " << name << " opened ================" << endl;
	if(t) {
		this->errRecovery->errQ->enqueue(lineno, colno, "Class already exists", name);
		return 0;
	}
	t = new Type();
	t->setName(name);
	m->reset();
	t->getScope()->parent = this->st->currScope;
	this->st->currScope->m->put(name, t, TYPE);
	this->st->currScope = t->getScope();
	
	cout << "Class " << name << " has been created\n";
	return t;
}
Type * MyParser::finishTypeDeclaration(Type* t) {
	// Creating default constructor for class if doesn't exist.
	Function* f = (Function*)this->st->currScope->m->get(t->getName());
	if (!f) {
		Function* f = new Function();
		f->setIsPublic(true);
		f->setIsConstructor(true);
		f->setName(t->getName());
		this->st->currScope->m->put(f->getName(), f, FUNCTION);
		cout << "==========================================================\n";
		cout << "Default constructor has been created with name: " << f->getName() << endl;
		cout << "==========================================================\n";
	}
	this->st->currScope = this->st->currScope->parent;
	if (t) {
		cout << "=============== Class " << t->getName() << " closed ================" << endl;
	}
	return t;
}

//========= Functions ===========
Function * MyParser::createFunction(char* name, int lineno, int colno, Modifier* m) {
	Function* f = (Function*)this->st->currScope->m->get(name);
	if (f) {
		cout << "========================================\n";
		cout << "Error[" << lineno << ", " << colno << "]: Function " << name << " already exists\n";
		cout << "========================================\n";
		this->errRecovery->errQ->enqueue(lineno, colno, "Function already exists ", name);
		return 0;
	}

	f = new Function();

	if (!this->setMethodData(f, name, m, lineno, colno)) {
		// Resetting the modifier
		m->reset();
		return 0;
	}

	// Resetting the modifier
	m->reset();

	// Printing function details
	f->printDetails();

	// Move to new scope
	f->getScope()->parent = this->st->currScope;
	this->st->currScope->m->put(name, f, FUNCTION);
	this->st->currScope = f->getScope();

	// Return the function
	return f;
}
Function * MyParser::finishFunctionDeclaration(Function* f) {
	if (f)
		cout << "=============== Function " << f->getName() << " closed ================" << endl;
	this->st->currScope = this->st->currScope->parent;
	return f;
}

bool MyParser::setMethodData(Function* f, char* name, Modifier* m, int lineNo, int colNo) {
	// Setting function modifiers
	f->setName(name);
	f->setIsPublic(m->getIsPublic()); f->setIsPrivate(m->getIsPrivate()); f->setIsProtected(m->getIsProtected());
	// Modifiers are not explicitly written
	if (m->getIsPrivate() == false && m->getIsProtected() == false && m->getIsPublic() == false) {
		f->setIsPublic(true);
	}
	f->setIsStatic(m->getIsStatic()); f->setIsAbstract(m->getIsAbstract()); f->setIsFinal(m->getIsFinal());
	f->setIsTransient(m->getIsTransient()); f->setIsSynchronized(m->getIsSynchronized()); f->setIsVolatile(m->getIsVolatile());
	f->setIsNative(m->getIsNative());
	f->setReturnType(m->getReturnType());

	// Checking if function has different modifiers
	if (f->illegalCombinationOfModifiers()) {
		cout << "==================================================\n";
		cout << "Error[" << lineNo << ", " << colNo << "]: Illegal combination of modifiers\n";
		cout << "==================================================\n";
		return false;
	}

	// Checking if function is constructor
	Type* t = (Type*)this->st->currScope->parent->m->get(name);
	if (t) {
		if (strcmp(t->getName(), name) == 0 && (m->getReturnType() && !m->getReturnType()[0])) {
			f->setIsConstructor(true);
		}
		else {
			f->setIsConstructor(false);
		}
	}
	else {
		f->setIsConstructor(false);
	}
	return true;
}

char** MyParser::getNames() {
	return this->names;
}

void MyParser::setNames(char** namesArr) {
	this->names = namesArr;
}

void MyParser::addToNames(char* name) {
	for (int i = 0; i < (sizeof(this->names) / sizeof(**this->names)); i++)
	{
		if (this->names && this->names[i] && !this->names[i][0]) {
			strcat(this->names[i], name);
			return;
		}
	}
}

void MyParser::resetNames() {
	for (int i = 0; i < (sizeof(this->names) / sizeof(**this->names)); i++)
	{
		this->names[i][0] = '\0';
	}
}