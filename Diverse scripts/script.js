
//Javascript til at vise data på hjemmesiden.

function get_data(){
    fetch("http://192.168.0.103/front_log_access.php")
    .then(response => response.json())
    .then(data => {
        show_data(data.logs),
        access_show_data(data.access)
    })
    .catch(error => {
        console.error("Error, fik ikke hentet data",error);
    });
}

function show_data(logs){
    const dataContainer = document.getElementById("log_access");

    dataContainer.innerHTML = "";

    logs.forEach(item => {
        const dataItem = document.createElement("div");
        dataItem.classList.add("data-item");
        dataItem.textContent = `ID: ${item.id}, NAVN: ${item.navn} UID: ${item.uid_entered}, Pin entered: ${item.pin_entered}, Result: ${item.result}, Time stamp: ${item.timestamp}`;
        dataContainer.append(dataItem);
    });
}
function access_show_data(access){
    const dataContainer = document.getElementById("access");

    dataContainer.innerHTML = "";

    access.forEach(item => {
        const dataItem = document.createElement("div");
        dataItem.classList.add("data-item");
        dataItem.textContent = `ID: ${item.id}, NAVN: ${item.navn}`;
        dataContainer.append(dataItem);
    });
}

get_data();

setInterval(get_data,5000);