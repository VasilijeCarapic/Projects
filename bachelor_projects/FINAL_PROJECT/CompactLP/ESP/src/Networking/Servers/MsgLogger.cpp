#include "Networking/Servers/MsgLogger.h"

#include "Networking/Servers/DateAndTime.h"
#include "Parsers/CommandParser.h"

void MsgLogger::putMsgInBuffer(const String& msg)
{
    Date d;
    Time t;

    d.setDate();
    t.setTime();

    msgBuffer* newNode = new msgBuffer;

    newNode->msg = "[" + d.getDate() + ", " + t.getTime() + "] : " + msg;
    newNode->next = nullptr;

    newNode -> date = d.getDate();
    newNode -> time = t.getTime();

    if (head == nullptr)
    {
        head = newNode;
        tail = newNode;
    }
    else
    {
        tail->next = newNode;
        tail = newNode;
    }

    msgCount++;
    if (msgCount > MAX_MESSAGES)
    {
        msgBuffer* temp = head;    /* Remember the oldest message */
        head = head->next;         /* Move pointer onto the next one */
        delete temp;               /* Delete the oldest one from mem */
        msgCount--;
        
        /* Safety check */
        if (head == nullptr) {
            tail = nullptr;
        }
    }
}

void MsgLogger::freeList()
{
    msgBuffer* curr = head;

    while (curr != nullptr)
    {
        msgBuffer* next = curr->next;
        delete curr;
        curr = next;
    }

    head = nullptr;
    tail = nullptr;

    msgCount = 0;
}

String MsgLogger::buildLog()
{
    String page = "";

    msgBuffer* i = head;

    while (i != nullptr){ page += i->msg + "<br>"; i = i->next; }

    return page;
}


