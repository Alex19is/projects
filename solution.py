from anysystem import Context, Message, Process





# AT MOST ONCE ---------------------------------------------------------------------------------------------------------

class AtMostOnceSender(Process):
    def __init__(self, proc_id: str, receiver_id: str):
        self._id = proc_id
        self._receiver = receiver_id
        self._curr_msg_id = 1

    def on_local_message(self, msg: Message, ctx: Context):
        # receive message for delivery from local user
        code_msg = Message('MESSAGE', {'text' : msg['text'], 'id' : self._curr_msg_id})
        ctx.send(code_msg, self._receiver)
        self._curr_msg_id += 1

    def on_message(self, msg: Message, sender: str, ctx: Context):
        # process messages from receiver here
        pass

    def on_timer(self, timer_name: str, ctx: Context):
        # process fired timers here
        pass


class AtMostOnceReceiver(Process):
    def __init__(self, proc_id: str):
        self._id = proc_id
        self._received_id = []

    def on_local_message(self, msg: Message, ctx: Context):
        # not used in this task
        pass

    def on_message(self, msg: Message, sender: str, ctx: Context):
        # process messages from receiver
        # deliver message to local user with ctx.send_local()
        if msg._data['id'] not in self._received_id:
            self._received_id.append(msg._data['id'])
            msg_to_local = Message('MESSAGE', {'text' : msg['text']})
            ctx.send_local(msg_to_local)

    def on_timer(self, timer_name: str, ctx: Context):
        # process fired timers here
        pass


# AT LEAST ONCE --------------------------------------------------------------------------------------------------------

class AtLeastOnceSender(Process):
    def __init__(self, proc_id: str, receiver_id: str):
        self._id = proc_id
        self._receiver = receiver_id
        self._curr_msg_id = 1
        self._msg_history = {}

    def on_local_message(self, msg: Message, ctx: Context):
        # receive message for delivery from local user
        code_msg = Message('MESSAGE', {'text' : msg['text'], 'id' : self._curr_msg_id})
        self._msg_history[self._curr_msg_id] = code_msg
        ctx.send(code_msg, self._receiver)
        ctx.set_timer(str(self._curr_msg_id), 3)
        self._curr_msg_id += 1
       

    def on_message(self, msg: Message, sender: str, ctx: Context):
        # process messages from receiver here
        if 'id' in msg._data:
            ctx.cancel_timer(str(msg._data['id']))
            if msg._data['id'] in self._msg_history:
                del self._msg_history[msg._data['id']]
        

    def on_timer(self, timer_name: str, ctx: Context):
        # process fired timers here
        ctx.send(self._msg_history[int(timer_name)], self._receiver)
        ctx.set_timer(timer_name, 3)
        pass




class AtLeastOnceReceiver(Process):
    def __init__(self, proc_id: str):
        self._id = proc_id

    def on_local_message(self, msg: Message, ctx: Context):
        # not used in this task
        pass

    def on_message(self, msg: Message, sender: str, ctx: Context):
        # process messages from receiver
        # deliver message to local user with ctx.send_local()
        msg_to_local = Message('MESSAGE', {'text' : msg['text']})
        ctx.send(msg, sender)
        ctx.send_local(msg_to_local)

    def on_timer(self, timer_name: str, ctx: Context):
        # process fired timers here
        pass


# EXACTLY ONCE ---------------------------------------------------------------------------------------------------------

class ExactlyOnceSender(Process):
    def __init__(self, proc_id: str, receiver_id: str):
        self._id = proc_id
        self._receiver = receiver_id
        self._curr_msg_id = 1
        self._msg_history = {}

    def on_local_message(self, msg: Message, ctx: Context):
        # receive message for delivery from local user
        code_msg = Message('MESSAGE', {'text' : msg['text'], 'id' : self._curr_msg_id})
        self._msg_history[self._curr_msg_id] = code_msg
        ctx.send(code_msg, self._receiver)
        ctx.set_timer(str(self._curr_msg_id), 3)
        self._curr_msg_id += 1
       

    def on_message(self, msg: Message, sender: str, ctx: Context):
        # process messages from receiver here
        if 'id' in msg._data:
            ctx.cancel_timer(str(msg._data['id']))
            if msg._data['id'] in self._msg_history:
                del self._msg_history[msg._data['id']]
        

    def on_timer(self, timer_name: str, ctx: Context):
        # process fired timers here
        ctx.send(self._msg_history[int(timer_name)], self._receiver)
        ctx.set_timer(timer_name, 3)


class ExactlyOnceReceiver(Process):
    def __init__(self, proc_id: str):
        self._id = proc_id
        self._received_msgs = []

    def on_local_message(self, msg: Message, ctx: Context):
        # not used in this task
        pass

    def on_message(self, msg: Message, sender: str, ctx: Context):
        # process messages from receiver
        # deliver message to local user with ctx.send_local()
        if msg._data['id'] not in self._received_msgs:
            msg_to_local = Message('MESSAGE', {'text' : msg['text']})
            ctx.send_local(msg_to_local)
            self._received_msgs.append(msg._data['id'])
        ctx.send(msg, sender)

    def on_timer(self, timer_name: str, ctx: Context):
        # process fired timers here
        pass


# EXACTLY ONCE + ORDERED -----------------------------------------------------------------------------------------------

class ExactlyOnceOrderedSender(Process):
    def __init__(self, proc_id: str, receiver_id: str):
        self._id = proc_id
        self._receiver = receiver_id
        self._curr_msg_id = 1
        self._msg_history = {}

    def on_local_message(self, msg: Message, ctx: Context):
        # receive message for delivery from local user
        code_msg = Message('MESSAGE', {'text' : msg['text'], 'id' : self._curr_msg_id})
        self._msg_history[self._curr_msg_id] = code_msg
        ctx.send(code_msg, self._receiver)
        ctx.set_timer(str(self._curr_msg_id), 3)
        self._curr_msg_id += 1
       

    def on_message(self, msg: Message, sender: str, ctx: Context):
        # process messages from receiver here
        if 'id' in msg._data:
            ctx.cancel_timer(str(msg._data['id']))
            if msg._data['id'] in self._msg_history:
                del self._msg_history[msg._data['id']]


    def on_timer(self, timer_name: str, ctx: Context):
        # process fired timers here
        ctx.send(self._msg_history[int(timer_name)], self._receiver)
        ctx.set_timer(timer_name, 3)
        pass


class ExactlyOnceOrderedReceiver(Process):
    def __init__(self, proc_id: str):
        self._id = proc_id
        self._next_to_send = 1

    def on_local_message(self, msg: Message, ctx: Context):
        # not used in this task
        pass

    def on_message(self, msg: Message, sender: str, ctx: Context):
        # process messages from receiver
        # deliver message to local user with ctx.send_local()        
        if self._next_to_send == msg._data['id']:
            self._next_to_send += 1
            msg_to_local = Message('MESSAGE', {'text' : msg['text']})
            ctx.send_local(msg_to_local)
            ctx.send(msg, sender)

            return
        if self._next_to_send > msg._data['id']:
            ctx.send(msg, sender)

      
        

    def on_timer(self, timer_name: str, ctx: Context):
        # process fired timers here
        pass
