char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="de">
    <head>
        <meta charset="UTF-8">
        <title>LED Cube control center</title>
        <style>
            html,
            body {
                width: 100%;
                height: 100%;
                padding: 0;
                margin: 0;
                border: none;
                font-family: sans-serif;
                font-size: 1em;
            }

            header {
                margin: 0 0 4% 0;
                font-size: 3.5em;
                text-align: center;
                padding: 1em 5% 0.5em 5%;
                background-color: #1976d2;
                border: none;
                border-bottom: solid 1px #0d47a1;
                color: #fff;
            }

            #Content {
                min-height: calc(100vh - (1.5rem + 21px));
            }

            h1 {
                color: #1e88e5;
                margin: 2% 5% auto 5%;
            }

            ul {
                margin: 3% 5% 3% 5%;
                padding: 0;
                margin-bottom: calc(5% - 25px);
                list-style-type: none;
                display: flex;
                flex-direction: row;
                flex-wrap: wrap;
                align-content: space-between;
                align-items: flex-start;
            }

            ul li {
                font-size: 2em;
                padding: 10px;
                border: solid 1px #607d8b;
                margin: 0 25px 25px 0;
                align-self: flex-start;
                cursor: pointer;
            }

            footer {
                margin: 0;
                font-size: 1.5rem;
                line-height: 1.5rem;
                padding: 10px 5% 10px 5%;
                background-color: #1976d2;
                border: none;
                border-top: solid 1px #0d47a1;
                color: #fff;
            }

            input[type=range] {
                -webkit-appearance: none;
                width: 90%;
                margin: 2% 0 auto 5%;
            }

            input[type=range]:focus {
                outline: none;
            }

            input[type=range]::-webkit-slider-runnable-track {
                width: 100%;
                height: 9.5px;
                cursor: pointer;
                box-shadow: 1.4px 1.4px 5px #607d8b, 0px 0px 1.4px #607d8b;
                background: #1976d2;
                border-radius: 12.7px;
                border: 0px solid rgba(0, 0, 0, 0);
            }

            input[type=range]::-webkit-slider-thumb {
                box-shadow: 1px 1px 1px #607d8b, 0px 0px 1px #607d8b;
                border: 0.5px solid #607d8b;
                height: 20px;
                width: 20px;
                border-radius: 50px;
                background: #ffffff;
                cursor: pointer;
                -webkit-appearance: none;
                margin-top: -5.25px;
            }

            input[type=range]:focus::-webkit-slider-runnable-track {
                background: #378fe7;
            }

            input[type=range]::-moz-range-track {
                width: 100%;
                height: 9.5px;
                cursor: pointer;
                box-shadow: 1.4px 1.4px 5px #607d8b, 0px 0px 1.4px #607d8b;
                background: #1976d2;
                border-radius: 12.7px;
                border: 0px solid rgba(0, 0, 0, 0);
            }

            input[type=range]::-moz-range-thumb {
                box-shadow: 1px 1px 1px #607d8b, 0px 0px 1px #607d8b;
                border: 0.5px solid #607d8b;
                height: 20px;
                width: 20px;
                border-radius: 50px;
                background: #ffffff;
                cursor: pointer;
            }

            input[type=range]::-ms-track {
                width: 100%;
                height: 9.5px;
                cursor: pointer;
                background: transparent;
                border-color: transparent;
                color: transparent;
            }

            input[type=range]::-ms-fill-lower {
                background: #145ca4;
                border: 0px solid rgba(0, 0, 0, 0);
                border-radius: 25.4px;
                box-shadow: 1.4px 1.4px 5px #607d8b, 0px 0px 1.4px #607d8b;
            }

            input[type=range]::-ms-fill-upper {
                background: #1976d2;
                border: 0px solid rgba(0, 0, 0, 0);
                border-radius: 25.4px;
                box-shadow: 1.4px 1.4px 5px #607d8b, 0px 0px 1.4px #607d8b;
            }

            input[type=range]::-ms-thumb {
                box-shadow: 1px 1px 1px #607d8b, 0px 0px 1px #607d8b;
                border: 0.5px solid #607d8b;
                height: 20px;
                width: 20px;
                border-radius: 50px;
                background: #ffffff;
                cursor: pointer;
                height: 9.5px;
            }

            input[type=range]:focus::-ms-fill-lower {
                background: #1976d2;
            }

            input[type=range]:focus::-ms-fill-upper {
                background: #378fe7;
            }
        </style>
        <script>
            var brightness = 0;
            var mode = 0;
            
            window.onload = function()
            {
                var Buttons = document.getElementsByTagName("ul")[0].children;
                for (var i = 0; i < Buttons.length; i++) {
                    Buttons[i].addEventListener("click", ClicKHandler(i));
                }

                document.getElementById("brightness").addEventListener("change", function () {
                brightness = document.getElementById("brightness").value;
                post()
                });
            }

            function post() {
                fetch("/power?mode=" + mode + "&brightness=" + brightness);
            };
            
            function callback(k) {
                mode = k;
                post();
            }
            function ClicKHandler(i) {
                return function () { callback(i) };
            }
        </script>
    </head>
    <body>
        <div id="Content">
            <header>LED Cube Control Center</header>
            <h1>Birghtness:</h1>
            <p><input id="brightness" type="range" min="0" max="31" value="16"></p>
            <h1>Animations:</h1>
            <ul>
                <li>Random</li>
                <li>Rainbow</li>
                <li>Red - Green - Blue</li>
                <li>Uni</li>
                <li>Rotation</li>
                <li>Cubic Cube</li>
                <li>MakerFaire</li>
            </ul>
        </div>
        <footer>Created by Michael Hohenstein, Patrick Glo&szlig;</footer>
    </body>
</html>
)=====";
