%%%-------------------------------------------------------------------
%%% File    : ketama.erl
%%% Author  : Richard Jones <rj@last.fm>
%%% Description : Port driver for libketama hasing
%%%-------------------------------------------------------------------
-module(ketama).

-behaviour(gen_server).

%% API
-export([start_link/0, start_link/1, start_link/2, getserver/1]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
     terminate/2, code_change/3]).

-record(state, {port}).

%%====================================================================
%% API
%%====================================================================
%%--------------------------------------------------------------------
%% Function: start_link() -> {ok,Pid} | ignore | {error,Error}
%% Description: Starts the server
%%--------------------------------------------------------------------
start_link() ->
    start_link("/web/site/GLOBAL/ketama.servers").

start_link(ServersFile) ->
    start_link(ServersFile, "/usr/bin/ketama_erlang_driver").

start_link(ServersFile, BinPath) ->
    gen_server:start_link({local, ?MODULE}, ?MODULE, [ServersFile, BinPath], []).

getserver(Key) ->
    gen_server:call(?MODULE, {getserver, Key}).

%%====================================================================
%% gen_server callbacks
%%====================================================================

%%--------------------------------------------------------------------
%% Function: init(Args) -> {ok, State} |
%%                         {ok, State, Timeout} |
%%                         ignore               |
%%                         {stop, Reason}
%% Description: Initiates the server 
%% ServersFile: ketama.servers list
%% BinPath: path to ketama_erlang_driver binary
%%--------------------------------------------------------------------
init([ServersFile, BinPath]) ->
    Exe = BinPath ++ " " ++ ServersFile,
    Port = open_port({spawn, Exe}, [binary, {packet, 1}, use_stdio]),
    {ok, #state{port=Port}}.

%%--------------------------------------------------------------------
%% Function: %% handle_call(Request, From, State) -> {reply, Reply, State} |
%%                                      {reply, Reply, State, Timeout} |
%%                                      {noreply, State} |
%%                                      {noreply, State, Timeout} |
%%                                      {stop, Reason, Reply, State} |
%%                                      {stop, Reason, State}
%% Description: Handling call messages
%%--------------------------------------------------------------------
handle_call({getserver, Key}, _From, #state{port=Port} = State) ->
    Port ! {self(), {command, Key}},
    receive
        {Port, {data, Data}} ->
            {reply, Data, State}
        after 1000 -> % if it takes this long, you have serious issues.
            {stop, ketama_port_timeout, State}
    end.

%%--------------------------------------------------------------------
%% Function: handle_cast(Msg, State) -> {noreply, State} |
%%                                      {noreply, State, Timeout} |
%%                                      {stop, Reason, State}
%% Description: Handling cast messages
%%--------------------------------------------------------------------
handle_cast(_Msg, State) ->
    {noreply, State}.

%%--------------------------------------------------------------------
%% Function: handle_info(Info, State) -> {noreply, State} |
%%                                       {noreply, State, Timeout} |
%%                                       {stop, Reason, State}
%% Description: Handling all non call/cast messages
%%--------------------------------------------------------------------
handle_info({'EXIT', Port, Reason}, #state{port = Port} = State) ->
    {stop, {port_terminated, Reason}, State}.


%%--------------------------------------------------------------------
%% Function: terminate(Reason, State) -> void()
%% Description: This function is called by a gen_server when it is about to
%% terminate. It should be the opposite of Module:init/1 and do any necessary
%% cleaning up. When it returns, the gen_server terminates with Reason.
%% The return value is ignored.
%%--------------------------------------------------------------------
terminate({port_terminated, _Reason}, _State) ->
    ok;

terminate(_Reason, #state{port = Port} = _State) ->
    port_close(Port).

%%--------------------------------------------------------------------
%% Func: code_change(OldVsn, State, Extra) -> {ok, NewState}
%% Description: Convert process state when code is changed
%%--------------------------------------------------------------------
code_change(_OldVsn, State, _Extra) ->
    {ok, State}.

%%--------------------------------------------------------------------
%%% Internal functions
%%--------------------------------------------------------------------

