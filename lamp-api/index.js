const express = require("express")
const cors = require("cors")

const app = express()
const port = 6001


const colors = ["red", "yellow", "green", "blue", "purple"]
// {lampId: lampId, color: colors[1]}
const lampQueue = []
const lamps = [{id:"0f8fb3f6-88c2-11ec-a8a3-0242ac120002", users: [1]}]
const lampIds = ["0f8fb3f6-88c2-11ec-a8a3-0242ac120002"]
const users = [{id: 1, name: "Max van Hattum"}]


app.use(cors())

app.use(express.urlencoded({extended: true}))

app.use(express.json())

app.post("/user", function(req, res){
    let name = req.body.name
    let id = users.length -1
    const user = { id: 1, name: name }
    users.push(user)
})

app.post("/user/connect", function(req, res){
    //add user to lamp
    let lampId = req.body.lampId
    let userId = req.body.userId

    // check if lamp in lamps
    let lampPresent = false    
    for (const lamp of lamps) {
        if(lampId == lamp.id){
            lampPresent = true
            lamp.users.push(userId)
            break;
        }
    }

    if(!lampPresent){
        lamps.push({id: lampId, users:[userId]})
    }
})

app.post("/lamp/remove", function(req, res){
    //remove user from lamp
    let lampId = req.body.lampId
    let userId = req.body.userId

    for (const lamp of lamps) {
        if(lampId == lamp.id){
            index = lamp.users.indexOf(userId)
            lamp.users.splice(index, 1)
            break;
        }
    }
})

app.get("/turn-on/:lampId", function(req, res){
    // lamp asks if it needs to be on
    // check if it needs to be on
    // return true or false with color
    let lampId = req.params.lampId
    let on = false
    let color = ""

    lampQueue.forEach(function(item, index, object){
        if( item.lampId === lampId){
            on = true
            color = item.color
            object.splice(index, 1)
        }
    })
    return res.json({on: on, color:color})
})

app.post("/turn-on", function(req, res){
    let lampId = req.body.lampId
    let userId = req.body.userId
    let color = req.body.color

    if (!(lampIds.includes(lampId))){
        return res.json({message: "Lamp does not exist :O"})
    }

    if(lamps[0].id === lampId && lamps[0].users[0] === userId){
        // check if the user already submitted a request to turn lamp on
        let alreadyInQueue = false
        lampQueue.forEach(x => {
            if(x.lampId === lampId && x.userId === userId){
                alreadyInQueue = true                
            }
        })
        
        // If not already in queue register lamp turn on request
        if(!alreadyInQueue){
            lampQueue.push({lampId: lampId, userId: userId, color: colors[color-1]})
            return res.json({message: "Added lamp to be turned on :)"})
        } else {
            return res.json({message: "You are alreadyin line to turn the lamp on :)"})
        }
    }

    return res.json({message: "That lamp is not yours to turn on >:("})     
})


app.listen(port, () => console.log("Started the API"))